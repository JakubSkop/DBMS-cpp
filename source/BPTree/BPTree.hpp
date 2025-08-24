#pragma once

#include <variant>
#include <iostream>

#include "ProgrammingInterface.hpp"

class BPTree{
    public:
        DB::PageRowPairs search(DB::Page& node, const DB::KeyRange& range){
			DB::PageRowPairs out{};
			nodeSearch(node, range, out); 

			return out;
        }

		std::pair<size_t, size_t> findKeyRange(const DB::KeyArray& keys, size_t keyCount, const DB::KeyRange& range) {
			auto start = keys.begin();
			auto startIt = std::lower_bound(start, start + keyCount, range.first);
			auto endIt   = std::upper_bound(start, start + keyCount, range.second);

			return { size_t(startIt - start), size_t(endIt - start) };
		}

		void nodeSearch(DB::Page& node, const DB::KeyRange& range, DB::PageRowPairs& out){
			if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
				DB::NodePage& nodePage = *nodePagePtr;

				auto [startIndex, endIndex] = findKeyRange(nodePage.Keys, nodePage.KeyCount, range);

				for (size_t index{startIndex}; index <= endIndex; ++index) {
					DB::Page& childNode = getPage(nodePage.ChildPageIds[index]); 

					nodeSearch(childNode, range, out);
				}
					
			} else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
				leafSearch(*leafPtr, range, out); 
			}
		}

		void leafSearch(DB::LeafPage& node, const DB::KeyRange& range, DB::PageRowPairs& out){
        	auto [startIndex, endIndex] = findKeyRange(node.Keys, node.KeyCount, range);

			for(size_t i = startIndex; i < endIndex; ++i){
  				out.push_back(node.ChildPages[i]);
			}
        }


		DB::ID_Int insert(DB::Page& node, DB::Variant& newKey){
			if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
				DB::NodePage& nodePage = *nodePagePtr;

				DB::OptionalKeyPagePair raisedPair = nodeInsert(node, newKey);

				if (!raisedPair) return nodePage.PageID;

				DB::ID_Int newPageId = getNewPageId();
				DB::NodePage newRoot{};
				newRoot.PageID = newPageId;

				newRoot.KeyCount = 1;
				newRoot.Keys[0] = std::move(raisedPair->first);

				newRoot.ChildPageIds[0] = nodePage.PageID;
				newRoot.ChildPageIds[1] = raisedPair->second;

				writePage(newRoot);

				return newPageId;
			} 

			// SOMETHING WRONG 
		}

		DB::OptionalKeyPagePair insertKeyPagePair(DB::NodePage& node, DB::Variant& promotedKey, DB::ID_Int newChildPageId){
			auto nodeBegin = node.Keys.begin();
			auto childBegin = node.ChildPageIds.begin();

			auto keysIt = std::upper_bound(nodeBegin, nodeBegin + node.KeyCount, promotedKey);
			size_t pos{keysIt - nodeBegin};

			if (node.KeyCount < DB::BP_TREE_SIZE) {
				std::move_backward(
					nodeBegin + pos,
					nodeBegin + node.KeyCount,
					nodeBegin + node.KeyCount + 1
				);
				std::move_backward(
					childBegin + (pos + 1),
					childBegin + (node.KeyCount + 1),
					childBegin + (node.KeyCount + 2)
				);

				node.Keys[pos] = std::move(promotedKey);
				node.ChildPageIds[pos + 1] = newChildPageId;

				++node.KeyCount; 
				return std::nullopt;
			}
			
			DB::ID_Int newPageId = getNewPageId();
			DB::NodePage newNode{};
			newNode.PageID = newPageId;

			size_t midpoint{(DB::BP_TREE_SIZE + 1) / 2};
			if (pos > midpoint){

				std::move(
					nodeBegin + midpoint + 1,
					nodeBegin + pos,
					newNode.Keys.begin());
				std::move(
					childBegin + midpoint + 1,
					childBegin + pos + 1,
					newNode.ChildPageIds.begin());
				

				newNode.Keys[pos - midpoint - 1] = std::move(promotedKey);
        		newNode.ChildPageIds[pos - midpoint] = newChildPageId;


				if (pos < DB::BP_TREE_SIZE) {
					std::move(
						nodeBegin + pos,
						nodeBegin + DB::BP_TREE_SIZE,
						newNode.Keys.begin() + pos - midpoint);
					std::move(
						childBegin + pos + 1,
						childBegin + (DB::BP_TREE_SIZE + 1),
						newNode.ChildPageIds.begin() + (pos - midpoint + 1));
				}

			}else{
				std::move(
					nodeBegin + midpoint + 1,
					nodeBegin + DB::BP_TREE_SIZE,
					newNode.Keys.begin());
				std::move(
					childBegin + midpoint + 1,
					childBegin + (DB::BP_TREE_SIZE + 1),
					newNode.ChildPageIds.begin());


				std::move_backward(
					nodeBegin + pos,
					nodeBegin + midpoint,
					nodeBegin + (midpoint + 1)
				);
				std::move_backward(
					childBegin + (pos + 1),
					childBegin + (midpoint + 1),
					childBegin + (midpoint + 2)
				);

				node.Keys[pos] = std::move(promotedKey);
        		childBegin[pos + 1] = newChildPageId;
			}

			node.KeyCount = midpoint;
			newNode.KeyCount = DB::BP_TREE_SIZE - midpoint;

			writePage(newNode);

			return DB::OptionalKeyPagePair{std::make_pair(node.Keys[midpoint], newPageId)};				
		}

		DB::OptionalKeyPagePair nodeInsert(DB::Page& node, DB::Variant& newKey){
			if (auto nodePagePtr = std::get_if<DB::NodePage>(&node)) {
				DB::NodePage& nodePage = *nodePagePtr;

				auto keysStart = nodePage.Keys.begin();
				auto keyEnd = std::lower_bound(keysStart, keysStart + nodePage.KeyCount, newKey);
				size_t childIndex{keyEnd - keysStart};

				DB::Page& childNode = getPage(nodePage.ChildPageIds[childIndex]);

				DB::OptionalKeyPagePair raisedPair {nodeInsert(childNode, newKey)};
				if (!raisedPair) return std::nullopt;

				return insertKeyPagePair(nodePage, raisedPair->first, raisedPair->second);

			} else if (auto leafPtr = std::get_if<DB::LeafPage>(&node)) {
				return leafInsert(*leafPtr, newKey); 
			}

			// Something wrong

		}

		DB::OptionalKeyPagePair leafInsert(DB::LeafPage& node, DB::Variant& newKey) {
			auto leafBegin = node.Keys.begin();
			auto rowBegin  = node.ChildPages.begin();

			// Find insert position
			auto keysIt = std::upper_bound(leafBegin, leafBegin + node.KeyCount, newKey);
			size_t pos{keysIt - leafBegin};

			if (node.KeyCount < DB::BP_TREE_SIZE) {
				// Space available in leaf, just insert
				std::move_backward(
					leafBegin + pos,
					leafBegin + node.KeyCount,
					leafBegin + node.KeyCount + 1
				);
				std::move_backward(
					rowBegin + pos,
					rowBegin + node.KeyCount,
					rowBegin + node.KeyCount + 1
				);

				node.Keys[pos] = newKey;  // store key
				node.ChildPages[pos] = DB::PageRowPair{node.PageID, 0}; // you’ll need to bind the actual RowID here

				++node.KeyCount;
				return std::nullopt;
			}

			// --- Split required ---
			DB::ID_Int newPageId = getNewPageId();
			DB::LeafPage newLeaf{};
			newLeaf.PageID = newPageId;

			size_t midpoint = (DB::BP_TREE_SIZE + 1) / 2;

			if (pos >= midpoint) {
				// Move upper half to new leaf first
				std::move(
					leafBegin + midpoint,
					leafBegin + node.KeyCount,
					newLeaf.Keys.begin()
				);
				std::move(
					rowBegin + midpoint,
					rowBegin + node.KeyCount,
					newLeaf.ChildPages.begin()
				);

				size_t moved = node.KeyCount - midpoint;
				newLeaf.KeyCount = moved;

				// Insert new key into the new leaf
				size_t newPos = pos - midpoint;
				std::move_backward(
					newLeaf.Keys.begin() + newPos,
					newLeaf.Keys.begin() + moved,
					newLeaf.Keys.begin() + moved + 1
				);
				std::move_backward(
					newLeaf.ChildPages.begin() + newPos,
					newLeaf.ChildPages.begin() + moved,
					newLeaf.ChildPages.begin() + moved + 1
				);

				newLeaf.Keys[newPos] = newKey;
				newLeaf.ChildPages[newPos] = DB::PageRowPair{newPageId, 0}; // fill row info
				++newLeaf.KeyCount;

				node.KeyCount = midpoint;
			} else {
				// Insert into left leaf
				std::move(
					leafBegin + midpoint,
					leafBegin + node.KeyCount,
					newLeaf.Keys.begin()
				);
				std::move(
					rowBegin + midpoint,
					rowBegin + node.KeyCount,
					newLeaf.ChildPages.begin()
				);

				size_t moved = node.KeyCount - midpoint;
				newLeaf.KeyCount = moved;

				std::move_backward(
					leafBegin + pos,
					leafBegin + midpoint,
					leafBegin + midpoint + 1
				);
				std::move_backward(
					rowBegin + pos,
					rowBegin + midpoint,
					rowBegin + midpoint + 1
				);

				node.Keys[pos] = newKey;
				node.ChildPages[pos] = PageRowPair{node.PageID, 0}; // fill row info

				node.KeyCount = midpoint;
			}

			writePage(newLeaf);

			return DB::OptionalKeyPagePair{std::make_pair(newLeaf.Keys[0], newPageId)};
		}
};




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
