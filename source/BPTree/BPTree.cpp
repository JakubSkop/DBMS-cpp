#include "BPTree.hpp"


DB::PageRowPairs BPTree::search(DB::Page& root, const DB::KeyRange& range){
    DB::PageRowPairs out{};
    searchRecurse(root, range, out); 
    return out;
}

std::pair<size_t, size_t> BPTree::findKeyRange(const DB::KeyArray& keys, size_t keyCount, const DB::KeyRange& range) {
    auto start = keys.begin();
    auto startIt = std::lower_bound(start, start + keyCount, range.first);
    auto endIt   = std::upper_bound(start, start + keyCount, range.second);

    return { size_t(startIt - start), size_t(endIt - start) };
}

void BPTree::searchRecurse(DB::Page& node, const DB::KeyRange& range, DB::PageRowPairs& out){
    if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
        nodeSearch(*nodePagePtr, range, out); 					
    } else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
        leafSearch(*leafPtr, range, out); 
    }
}

void BPTree::nodeSearch(DB::NodePage& node, const DB::KeyRange& range, DB::PageRowPairs& out){
    auto [startIndex, endIndex] = findKeyRange(node.Keys, node.KeyCount, range);

    for (size_t index{startIndex}; index <= endIndex; ++index) {
        DB::Page& childNode = getPage(node.ChildPageIds[index]); 
        searchRecurse(childNode, range, out);
    }
}

void BPTree::leafSearch(DB::LeafPage& node, const DB::KeyRange& range, DB::PageRowPairs& out){
    auto [startIndex, endIndex] = findKeyRange(node.Keys, node.KeyCount, range);

    for(size_t i = startIndex; i < endIndex; ++i){
        out.push_back(node.ChildPages[i]);
    }
}


DB::ID_Int BPTree::insert(DB::Page& root, int newKey, DB::PageRowPair rowPosition){
    if (auto nodePagePtr = std::get_if<DB::NodePage>(&root)) {
        DB::NodePage& nodePage = *nodePagePtr;

        DB::OptionalKeyPagePair raisedPair = nodeInsert(nodePage, newKey, rowPosition);

        if (!raisedPair) return nodePage.PageID;

        return createNewRoot(raisedPair->first, nodePage.PageID, raisedPair->second);
    } 

    throw std::runtime_error("Passed non B+Node PageId");
}

DB::OptionalKeyPagePair BPTree::insertRecurse(DB::Page& node, int newKey, DB::PageRowPair rowPosition){
    if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
        return nodeInsert(*nodePagePtr, newKey, rowPosition);

    } else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
        return leafInsert(*leafPtr, newKey, rowPosition); 
    }

    throw std::runtime_error("Encountered non B+ PageId");
}

DB::OptionalKeyPagePair BPTree::nodeInsert(DB::NodePage& node, int newKey, DB::PageRowPair rowPosition){
    auto keysStart = node.Keys.begin();
    auto keyEnd = std::lower_bound(keysStart, keysStart + node.KeyCount, newKey);
    size_t childIndex{keyEnd - keysStart};

    DB::Page& childNode = getPage(node.ChildPageIds[childIndex]);

    DB::OptionalKeyPagePair raisedPair {insertRecurse(childNode, newKey, rowPosition)};
    if (!raisedPair) return std::nullopt;

    return insertKeyPagePair(node, raisedPair->first, raisedPair->second);
}

DB::OptionalKeyPagePair BPTree::insertKeyPagePair(DB::NodePage& node, int promotedKey, DB::ID_Int newChildPageId){
    size_t origKeys = node.KeyCount;

    std::vector<int> tmpKeys; tmpKeys.reserve(origKeys + 1);
    std::vector<DB::ID_Int> tmpChildren; tmpChildren.reserve(origKeys + 2);
    
    for (size_t i = 0; i < origKeys; ++i) tmpKeys.push_back(node.Keys[i]);
    for (size_t i = 0; i < origKeys + 1; ++i) tmpChildren.push_back(node.ChildPageIds[i]);


    auto nodeBegin = node.Keys.begin();
    auto keysIt = std::upper_bound(nodeBegin, nodeBegin + node.KeyCount, promotedKey);
    size_t pos{keysIt - nodeBegin};
    
    tmpKeys.insert(tmpKeys.begin() + pos, promotedKey);
    tmpChildren.insert(tmpChildren.begin() + pos + 1, newChildPageId);
    

    if (tmpKeys.size() <= DB::BP_TREE_SIZE) {
        node.KeyCount = tmpKeys.size();
        for (size_t i = 0; i < node.KeyCount; ++i) node.Keys[i] = tmpKeys[i];
        for (size_t i = 0; i < node.KeyCount + 1; ++i) node.ChildPageIds[i] = tmpChildren[i];

        return std::nullopt;
    }
    
    
    size_t totalKeys = tmpKeys.size();
    size_t midIndex = totalKeys / 2;

    int keyToPromote = tmpKeys[midIndex];

    DB::ID_Int newPageId = getNewPageId();
    DB::NodePage rightNode{};
    rightNode.PageID = newPageId;

    node.KeyCount = midIndex;
    for (size_t i = 0; i < node.KeyCount; ++i) node.Keys[i] = tmpKeys[i];
    for (size_t i = 0; i < node.KeyCount + 1; ++i) node.ChildPageIds[i] = tmpChildren[i];

    size_t rightKeys = totalKeys - (midIndex + 1);
    rightNode.KeyCount = rightKeys;
    for (size_t i = 0; i < rightKeys; ++i) rightNode.Keys[i] = tmpKeys[midIndex + 1 + i];
    for (size_t i = 0; i < rightKeys + 1; ++i) rightNode.ChildPageIds[i] = tmpChildren[midIndex + 1 + i];

    writePage(node);
    writePage(rightNode);

    return DB::OptionalKeyPagePair{ std::make_pair(keyToPromote, newPageId) };			
}

DB::OptionalKeyPagePair BPTree::leafInsert(DB::LeafPage& node, int newKey, DB::PageRowPair rowPosition) {
    auto leafBegin = node.Keys.begin();
    auto keysIt = std::upper_bound(leafBegin, leafBegin + node.KeyCount, newKey);
    size_t pos{keysIt - leafBegin};


    std::vector<int> tmpKeys; tmpKeys.reserve(node.KeyCount + 1);
    std::vector<DB::PageRowPair> tmpRows; tmpRows.reserve(node.KeyCount + 1);

    for (size_t i = 0; i < node.KeyCount; ++i) {
        tmpKeys.push_back(node.Keys[i]);
        tmpRows.push_back(node.ChildPages[i]);
    }


    tmpKeys.insert(tmpKeys.begin() + pos, newKey);
    tmpRows.insert(tmpRows.begin() + pos, rowPosition);

    if (tmpKeys.size() <= DB::BP_TREE_SIZE) {
        node.KeyCount = tmpKeys.size();
        for (size_t i = 0; i < node.KeyCount; ++i) {
            node.Keys[i] = tmpKeys[i];
            node.ChildPages[i] = tmpRows[i];
        }
        return std::nullopt;
    }

    size_t totalKeys = tmpKeys.size();
    size_t midIndex = totalKeys / 2; 

    DB::ID_Int newPageId = getNewPageId();
    DB::LeafPage rightLeaf{};
    rightLeaf.PageID = newPageId;

    node.KeyCount = midIndex;
    for (size_t i = 0; i < node.KeyCount; ++i) {
        node.Keys[i] = tmpKeys[i];
        node.ChildPages[i] = tmpRows[i];
    }

    size_t rightKeys = totalKeys - midIndex;
    rightLeaf.KeyCount = rightKeys;
    for (size_t i = 0; i < rightKeys; ++i) {
        rightLeaf.Keys[i] = tmpKeys[midIndex + i];
        rightLeaf.ChildPages[i] = tmpRows[midIndex + i];
    }

    writePage(node);
    writePage(rightLeaf);

    int promoteKey = rightLeaf.Keys[0];
    return DB::OptionalKeyPagePair{ std::make_pair(promoteKey, newPageId) };
}

DB::ID_Int BPTree::createNewRoot(int key, DB::ID_Int childOne, DB::ID_Int childTwo){
    DB::ID_Int newPageId = getNewPageId();
    DB::NodePage newRoot{};
    newRoot.PageID = newPageId;

    newRoot.KeyCount = 1;
    newRoot.Keys[0] = key;

    newRoot.ChildPageIds[0] = childOne;
    newRoot.ChildPageIds[1] = childTwo;

    writePage(newRoot);

    return newPageId;
}
