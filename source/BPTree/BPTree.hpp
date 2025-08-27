#pragma once

#include <variant>
#include <iostream>
#include <exception>

#include "ProgrammingInterface.hpp"

class BPTree {
public:
	DB::PageRowPairs search(DB::Page& root, const DB::KeyRange& range);
	DB::ID_Int insert(DB::Page& root, int newKey, DB::PageRowPair rowPosition);


	private:
	std::pair<size_t, size_t> findKeyRange(const DB::KeyArray& keys, size_t keyCount, const DB::KeyRange& range);
	void searchRecurse(DB::Page& node, const DB::KeyRange& range, DB::PageRowPairs& out);
	void nodeSearch(DB::NodePage& node, const DB::KeyRange& range, DB::PageRowPairs& out);
	void leafSearch(DB::LeafPage& node, const DB::KeyRange& range, DB::PageRowPairs& out);

	DB::OptionalKeyPagePair insertRecurse(DB::Page& node, int newKey, DB::PageRowPair rowPosition);
	DB::OptionalKeyPagePair nodeInsert(DB::NodePage& node, int newKey, DB::PageRowPair rowPosition);
	DB::OptionalKeyPagePair insertKeyPagePair(DB::NodePage& node, int promotedKey, DB::ID_Int newChildPageId);
	DB::OptionalKeyPagePair leafInsert(DB::LeafPage& node, int newKey, DB::PageRowPair rowPosition);
	DB::ID_Int createNewRoot(int key, DB::ID_Int childOne, DB::ID_Int childTwo);
};











































































































// class BPTree{
//     public:
//         DB::PageRowPairs search(DB::Page& root, const DB::KeyRange& range){
// 			DB::PageRowPairs out{};
// 			searchRecurse(root, range, out); 
// 			return out;
//         }

// 		std::pair<size_t, size_t> findKeyRange(const DB::KeyArray& keys, size_t keyCount, const DB::KeyRange& range) {
// 			auto start = keys.begin();
// 			auto startIt = std::lower_bound(start, start + keyCount, range.first);
// 			auto endIt   = std::upper_bound(start, start + keyCount, range.second);

// 			return { size_t(startIt - start), size_t(endIt - start) };
// 		}

// 		void searchRecurse(DB::Page& node, const DB::KeyRange& range, DB::PageRowPairs& out){
// 			if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
// 				nodeSearch(*nodePagePtr, range, out); 					
// 			} else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
// 				leafSearch(*leafPtr, range, out); 
// 			}
// 		}
		
// 		void nodeSearch(DB::NodePage& node, const DB::KeyRange& range, DB::PageRowPairs& out){
// 			auto [startIndex, endIndex] = findKeyRange(node.Keys, node.KeyCount, range);

// 			for (size_t index{startIndex}; index <= endIndex; ++index) {
// 				DB::Page& childNode = getPage(node.ChildPageIds[index]); 
// 				searchRecurse(childNode, range, out);
// 			}
// 		}

// 		void leafSearch(DB::LeafPage& node, const DB::KeyRange& range, DB::PageRowPairs& out){
//         	auto [startIndex, endIndex] = findKeyRange(node.Keys, node.KeyCount, range);

// 			for(size_t i = startIndex; i < endIndex; ++i){
//   				out.push_back(node.ChildPages[i]);
// 			}
//         }


// 		DB::ID_Int insert(DB::Page& root, int newKey, DB::PageRowPair rowPosition){
// 			if (auto nodePagePtr = std::get_if<DB::NodePage>(&root)) {
// 				DB::NodePage& nodePage = *nodePagePtr;

// 				DB::OptionalKeyPagePair raisedPair = nodeInsert(nodePage, newKey, rowPosition);

// 				if (!raisedPair) return nodePage.PageID;

// 				return createNewRoot(raisedPair->first, nodePage.PageID, raisedPair->second);
// 			} 

// 			throw std::runtime_error("Passed non B+Node PageId");
// 		}

// 		DB::OptionalKeyPagePair insertRecurse(DB::Page& node, int newKey, DB::PageRowPair rowPosition){
// 			if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
// 				return nodeInsert(*nodePagePtr, newKey, rowPosition);

// 			} else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
// 				return leafInsert(*leafPtr, newKey, rowPosition); 
// 			}

// 			throw std::runtime_error("Encountered non B+ PageId");
// 		}

// 		DB::OptionalKeyPagePair nodeInsert(DB::NodePage& node, int newKey, DB::PageRowPair rowPosition){
// 			auto keysStart = node.Keys.begin();
// 			auto keyEnd = std::lower_bound(keysStart, keysStart + node.KeyCount, newKey);
// 			size_t childIndex{keyEnd - keysStart};

// 			DB::Page& childNode = getPage(node.ChildPageIds[childIndex]);

// 			DB::OptionalKeyPagePair raisedPair {insertRecurse(childNode, newKey, rowPosition)};
// 			if (!raisedPair) return std::nullopt;

// 			return insertKeyPagePair(node, raisedPair->first, raisedPair->second);
// 		}

// 		DB::OptionalKeyPagePair insertKeyPagePair(DB::NodePage& node, int promotedKey, DB::ID_Int newChildPageId){
// 			size_t origKeys = node.KeyCount;

//     		std::vector<int> tmpKeys; tmpKeys.reserve(origKeys + 1);
//     		std::vector<DB::ID_Int> tmpChildren; tmpChildren.reserve(origKeys + 2);
			
//     		for (size_t i = 0; i < origKeys; ++i) tmpKeys.push_back(node.Keys[i]);
//    			for (size_t i = 0; i < origKeys + 1; ++i) tmpChildren.push_back(node.ChildPageIds[i]);


// 			auto nodeBegin = node.Keys.begin();
// 			auto keysIt = std::upper_bound(nodeBegin, nodeBegin + node.KeyCount, promotedKey);
// 			size_t pos{keysIt - nodeBegin};
			
// 			tmpKeys.insert(tmpKeys.begin() + pos, promotedKey);
//     		tmpChildren.insert(tmpChildren.begin() + pos + 1, newChildPageId);
			

// 			if (tmpKeys.size() <= DB::BP_TREE_SIZE) {
// 				node.KeyCount = tmpKeys.size();
// 				for (size_t i = 0; i < node.KeyCount; ++i) node.Keys[i] = tmpKeys[i];
// 				for (size_t i = 0; i < node.KeyCount + 1; ++i) node.ChildPageIds[i] = tmpChildren[i];

// 				return std::nullopt;
// 			}
			
			
// 			size_t totalKeys = tmpKeys.size();
// 			size_t midIndex = totalKeys / 2;

// 			int keyToPromote = tmpKeys[midIndex];

// 			DB::ID_Int newPageId = getNewPageId();
// 			DB::NodePage rightNode{};
// 			rightNode.PageID = newPageId;

// 			node.KeyCount = midIndex;
// 			for (size_t i = 0; i < node.KeyCount; ++i) node.Keys[i] = tmpKeys[i];
// 			for (size_t i = 0; i < node.KeyCount + 1; ++i) node.ChildPageIds[i] = tmpChildren[i];

// 			size_t rightKeys = totalKeys - (midIndex + 1);
// 			rightNode.KeyCount = rightKeys;
// 			for (size_t i = 0; i < rightKeys; ++i) rightNode.Keys[i] = tmpKeys[midIndex + 1 + i];
// 			for (size_t i = 0; i < rightKeys + 1; ++i) rightNode.ChildPageIds[i] = tmpChildren[midIndex + 1 + i];

// 			writePage(node);
// 			writePage(rightNode);

// 			return DB::OptionalKeyPagePair{ std::make_pair(keyToPromote, newPageId) };			
// 		}

// 		DB::OptionalKeyPagePair leafInsert(DB::LeafPage& node, int newKey, DB::PageRowPair rowPosition) {
// 			auto leafBegin = node.Keys.begin();
// 			auto keysIt = std::upper_bound(leafBegin, leafBegin + node.KeyCount, newKey);
// 			size_t pos{keysIt - leafBegin};


// 			std::vector<int> tmpKeys; tmpKeys.reserve(node.KeyCount + 1);
// 			std::vector<DB::PageRowPair> tmpRows; tmpRows.reserve(node.KeyCount + 1);

// 			for (size_t i = 0; i < node.KeyCount; ++i) {
// 				tmpKeys.push_back(node.Keys[i]);
// 				tmpRows.push_back(node.ChildPages[i]);
// 			}


// 			tmpKeys.insert(tmpKeys.begin() + pos, newKey);
// 			tmpRows.insert(tmpRows.begin() + pos, rowPosition);

// 			if (tmpKeys.size() <= DB::BP_TREE_SIZE) {
// 				node.KeyCount = tmpKeys.size();
// 				for (size_t i = 0; i < node.KeyCount; ++i) {
// 					node.Keys[i] = tmpKeys[i];
// 					node.ChildPages[i] = tmpRows[i];
// 				}
// 				return std::nullopt;
// 			}

// 			size_t totalKeys = tmpKeys.size();
// 			size_t midIndex = totalKeys / 2; 

// 			DB::ID_Int newPageId = getNewPageId();
// 			DB::LeafPage rightLeaf{};
// 			rightLeaf.PageID = newPageId;

// 			node.KeyCount = midIndex;
// 			for (size_t i = 0; i < node.KeyCount; ++i) {
// 				node.Keys[i] = tmpKeys[i];
// 				node.ChildPages[i] = tmpRows[i];
// 			}

// 			size_t rightKeys = totalKeys - midIndex;
// 			rightLeaf.KeyCount = rightKeys;
// 			for (size_t i = 0; i < rightKeys; ++i) {
// 				rightLeaf.Keys[i] = tmpKeys[midIndex + i];
// 				rightLeaf.ChildPages[i] = tmpRows[midIndex + i];
// 			}

// 			writePage(node);
// 			writePage(rightLeaf);

// 			int promoteKey = rightLeaf.Keys[0];
// 			return DB::OptionalKeyPagePair{ std::make_pair(promoteKey, newPageId) };
// 		}

// 		DB::ID_Int createNewRoot(int key, DB::ID_Int childOne, DB::ID_Int childTwo){
// 			DB::ID_Int newPageId = getNewPageId();
// 			DB::NodePage newRoot{};
// 			newRoot.PageID = newPageId;

// 			newRoot.KeyCount = 1;
// 			newRoot.Keys[0] = key;

// 			newRoot.ChildPageIds[0] = childOne;
// 			newRoot.ChildPageIds[1] = childTwo;

// 			writePage(newRoot);

// 			return newPageId;
// 		}
// };















			// auto leafBegin = node.Keys.begin();
			// auto rowBegin  = node.ChildPages.begin();

			// // Find insert position
			// auto keysIt = std::upper_bound(leafBegin, leafBegin + node.KeyCount, newKey);
			// size_t pos{keysIt - leafBegin};

			// if (node.KeyCount < DB::BP_TREE_SIZE) {
			// 	// Space available in leaf, just insert
			// 	std::move_backward(
			// 		leafBegin + pos,
			// 		leafBegin + node.KeyCount,
			// 		leafBegin + node.KeyCount + 1
			// 	);
			// 	std::move_backward(
			// 		rowBegin + pos,
			// 		rowBegin + node.KeyCount,
			// 		rowBegin + node.KeyCount + 1
			// 	);

			// 	node.Keys[pos] = newKey;  // store key
			// 	node.ChildPages[pos] = rowPosition; // you’ll need to bind the actual RowID here

			// 	++node.KeyCount;
			// 	return std::nullopt;
			// }

			// // --- Split required ---
			// DB::ID_Int newPageId = getNewPageId();
			// DB::LeafPage newLeaf{};
			// newLeaf.PageID = newPageId;

			// size_t midpoint = (DB::BP_TREE_SIZE + 1) / 2;

			// if (pos >= midpoint) {
			// 	// Move upper half to new leaf first
			// 	std::move(
			// 		leafBegin + midpoint,
			// 		leafBegin + node.KeyCount,
			// 		newLeaf.Keys.begin()
			// 	);
			// 	std::move(
			// 		rowBegin + midpoint,
			// 		rowBegin + node.KeyCount,
			// 		newLeaf.ChildPages.begin()
			// 	);

			// 	size_t moved = node.KeyCount - midpoint;
			// 	newLeaf.KeyCount = moved;

			// 	// Insert new key into the new leaf
			// 	size_t newPos = pos - midpoint;
			// 	std::move_backward(
			// 		newLeaf.Keys.begin() + newPos,
			// 		newLeaf.Keys.begin() + moved,
			// 		newLeaf.Keys.begin() + moved + 1
			// 	);
			// 	std::move_backward(
			// 		newLeaf.ChildPages.begin() + newPos,
			// 		newLeaf.ChildPages.begin() + moved,
			// 		newLeaf.ChildPages.begin() + moved + 1
			// 	);

			// 	newLeaf.Keys[newPos] = newKey;
			// 	newLeaf.ChildPages[newPos] = DB::PageRowPair{newPageId, 0}; // fill row info
			// 	++newLeaf.KeyCount;

			// 	node.KeyCount = midpoint;
			// } else {
			// 	// Insert into left leaf
			// 	std::move(
			// 		leafBegin + midpoint,
			// 		leafBegin + node.KeyCount,
			// 		newLeaf.Keys.begin()
			// 	);
			// 	std::move(
			// 		rowBegin + midpoint,
			// 		rowBegin + node.KeyCount,
			// 		newLeaf.ChildPages.begin()
			// 	);

			// 	size_t moved = node.KeyCount - midpoint;
			// 	newLeaf.KeyCount = moved;

			// 	std::move_backward(
			// 		leafBegin + pos,
			// 		leafBegin + midpoint,
			// 		leafBegin + midpoint + 1
			// 	);
			// 	std::move_backward(
			// 		rowBegin + pos,
			// 		rowBegin + midpoint,
			// 		rowBegin + midpoint + 1
			// 	);

			// 	node.Keys[pos] = newKey;
			// 	node.ChildPages[pos] = PageRowPair{node.PageID, 0}; // fill row info

			// 	node.KeyCount = midpoint;
			// }

			// writePage(newLeaf);

			// return DB::OptionalKeyPagePair{std::make_pair(newLeaf.Keys[0], newPageId)};











			
			
			
			
			// auto nodeBegin = node.Keys.begin();
			// auto childBegin = node.ChildPageIds.begin();

			// auto keysIt = std::upper_bound(nodeBegin, nodeBegin + node.KeyCount, promotedKey);
			// size_t pos{keysIt - nodeBegin};

			// if (node.KeyCount < DB::BP_TREE_SIZE) {
			// 	std::move_backward(
			// 		nodeBegin + pos,
			// 		nodeBegin + node.KeyCount,
			// 		nodeBegin + node.KeyCount + 1
			// 	);
			// 	std::move_backward(
			// 		childBegin + (pos + 1),
			// 		childBegin + (node.KeyCount + 1),
			// 		childBegin + (node.KeyCount + 2)
			// 	);

			// 	node.Keys[pos] = promotedKey;
			// 	node.ChildPageIds[pos + 1] = newChildPageId;

			// 	++node.KeyCount; 
			// 	return std::nullopt;
			// }
			
			// DB::ID_Int newPageId = getNewPageId();
			// DB::NodePage newNode{};
			// newNode.PageID = newPageId;

			// size_t midpoint{(DB::BP_TREE_SIZE + 1) / 2};
			// if (pos > midpoint){

			// 	std::move(
			// 		nodeBegin + midpoint + 1,
			// 		nodeBegin + pos,
			// 		newNode.Keys.begin());
			// 	std::move(
			// 		childBegin + midpoint + 1,
			// 		childBegin + pos + 1,
			// 		newNode.ChildPageIds.begin());
				

			// 	newNode.Keys[pos - midpoint - 1] = promotedKey;
        	// 	newNode.ChildPageIds[pos - midpoint] = newChildPageId;


			// 	if (pos < DB::BP_TREE_SIZE) {
			// 		std::move(
			// 			nodeBegin + pos,
			// 			nodeBegin + DB::BP_TREE_SIZE,
			// 			newNode.Keys.begin() + pos - midpoint);
			// 		std::move(
			// 			childBegin + pos + 1,
			// 			childBegin + (DB::BP_TREE_SIZE + 1),
			// 			newNode.ChildPageIds.begin() + (pos - midpoint + 1));
			// 	}

			// }else{
			// 	std::move(
			// 		nodeBegin + midpoint + 1,
			// 		nodeBegin + DB::BP_TREE_SIZE,
			// 		newNode.Keys.begin());
			// 	std::move(
			// 		childBegin + midpoint + 1,
			// 		childBegin + (DB::BP_TREE_SIZE + 1),
			// 		newNode.ChildPageIds.begin());


			// 	std::move_backward(
			// 		nodeBegin + pos,
			// 		nodeBegin + midpoint,
			// 		nodeBegin + (midpoint + 1)
			// 	);
			// 	std::move_backward(
			// 		childBegin + (pos + 1),
			// 		childBegin + (midpoint + 1),
			// 		childBegin + (midpoint + 2)
			// 	);

			// 	node.Keys[pos] = promotedKey;
        	// 	childBegin[pos + 1] = newChildPageId;
			// }

			// node.KeyCount = midpoint;
			// newNode.KeyCount = DB::BP_TREE_SIZE - midpoint;

			// writePage(newNode);

			// return DB::OptionalKeyPagePair{std::make_pair(node.Keys[midpoint], newPageId)};	



			// auto start{node.Keys.begin()};
			// auto range{std::equal_range(start, start + node.KeyCount, val)};

			// size_t leftIndex{range.first - start};
			// size_t rightIndex{range.second - start};

			// DB::PageRowPairs pairs{};
			// for(size_t i = leftIndex; i < rightIndex; ++i){
  			// 	pairs.push_back(node.ChildPages[i]);

			// }
			// return pairs;





			// auto start{node.Keys.begin()};
			// auto range{std::equal_range(start, start + node.KeyCount, val)};

			// size_t leftIndex{range.first - start};
			// size_t rightIndex{range.second - start};
			
			// DB::PageRowPairs pairs{};
			// const DB::Page& childNode = getPage(node.ChildPageIds[index]);
			// return search(childNode, val);





            // size_t lo{0};
            // size_t hi{node.KeyCount - 2};

            // while(lo <= hi) {
            //     size_t mid{(hi - lo) / 2 + lo};

            //     if(node.Keys[mid] < val){
            //         if (mid == 0) {
            //             const DB::NodePage& childNode = getPage(node.ChildPageIds[mid]);
            //             return search(childNode, val);
            //         }
            //         lo = mid + 1;

            //     }else if (val <= node.Keys[mid + 1]){
            //         if (mid + 2 == node.KeyCount) {
            //             const DB::NodePage& childNode = getPage(node.ChildPageIds[mid + 2]);
            //             return search(childNode, val);
            //         }
            //         hi = mid - 1;

            //     }else{
            //         const DB::NodePage& childNode = getPage(node.ChildPageIds[mid]);
            //         return search(childNode, val);
            //     }
            // }




			
			
            // size_t lo{0};
            // size_t hi{node.KeyCount - 1};
			// size_t lastL{};

            // while(lo <= hi) {
            //     size_t mid{(hi - lo) / 2 + lo};

            //     if(node.Keys[mid] <= val){
			// 		lastL = mid;
            //         lo = mid + 1;

            //     }else{
            //         hi = mid - 1;

            //     }
            // }

			// size_t lo{0};
            // size_t hi{node.KeyCount - 1};
			// size_t lastR{};

            // while(lo <= hi) {
            //     size_t mid{(hi - lo) / 2 + lo};

            //     if(node.Keys[mid] < val){
            //         lo = mid + 1;

            //     }else{
			// 		lastR = mid;
            //         hi = mid - 1;
            //     }
            // }
			// DB::PageRowPairs pairs;
			// for(size_t i = lastL; i <= lastR; ++i){
  			// 	pairs.push_back(node.ChildPages[i]);

			// }
			// return pairs;
