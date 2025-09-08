#include "Table.hpp"

using namespace DB;


Page::Page(ID_Int Id) : pageId{Id} {};

ID_Int Page::getId() const{
    return pageId;
};
            
const Page::Inner& Page::getData() const{
    return data;
};
            
Page::Inner& Page::getData(){
    dirty = true;
    return data;
};






Serializer::Serializer(const std::string& filename) : file{filename, file.binary | file.trunc | file.in | file.out}{};

Header Serializer::getHeader(){
    Header h;
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&h.NumberOfColumns), sizeof(h.NumberOfColumns));
    file.read(reinterpret_cast<char*>(&h.ColumnTypes), sizeof(h.ColumnTypes));
    file.read(reinterpret_cast<char*>(&h.ColumnNames), sizeof(h.ColumnNames));
    file.read(reinterpret_cast<char*>(&h.TableName), sizeof(h.TableName));
    file.read(reinterpret_cast<char*>(&h.FirstPageID), sizeof(h.FirstPageID));
    file.read(reinterpret_cast<char*>(&h.LastPageID), sizeof(h.LastPageID));
    file.read(reinterpret_cast<char*>(&h.RootID), sizeof(h.RootID));
    return h; 
};

void Serializer::writeHeader(Header h){
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&h.NumberOfColumns), sizeof(h.NumberOfColumns));
    file.write(reinterpret_cast<char*>(&h.ColumnTypes), sizeof(h.ColumnTypes));
    file.write(reinterpret_cast<char*>(&h.ColumnNames), sizeof(h.ColumnNames));
    file.write(reinterpret_cast<char*>(&h.TableName), sizeof(h.TableName));
    file.write(reinterpret_cast<char*>(&h.FirstPageID), sizeof(h.FirstPageID));
    file.write(reinterpret_cast<char*>(&h.LastPageID), sizeof(h.LastPageID));
    file.write(reinterpret_cast<char*>(&h.RootID), sizeof(h.RootID));
}

Page Serializer::getPage(ID_Int PageID){ //Wrong for now
    file.seekg(static_cast<std::streamoff>(HEADER_SIZE + PageID*PAGE_SIZE));
    Page p(PageID);
    TypeInt PageType;
    file.seekg(sizeof(PageID), std::ios::cur);
    file.read(reinterpret_cast<char*>(&PageType), sizeof(PageType));

    switch (PageType){
        case 0: //Data Page
        {
            DataPage_Inner inner{};
            file.read(reinterpret_cast<char*>(&inner.NextPageID), sizeof(inner.NextPageID));
            file.read(reinterpret_cast<char*>(&inner.NumberOfRows), sizeof(inner.NumberOfRows));
            auto types = currentHeaderPtr->ColumnTypes;
            for (size_t i = 0; i < currentHeaderPtr->NumberOfColumns; i++){
                std::visit([&](auto &var){file.read(reinterpret_cast<char*>(var.data()), static_cast<std::streamsize>(inner.NumberOfRows*sizeof(MappingStruct<>::MappingArray[types[i]])));}, inner.columns[i].data);
                //file.read(reinterpret_cast<char*>(var.data()), inner.NumberOfRows*sizeof(MappingStruct<>::MappingArray[types[i]]));
            }
        } break; 
        case 1:
        {
            NodePage_Inner inner{};
            file.read(reinterpret_cast<char*>(&inner.ParentPageID), sizeof(inner.ParentPageID));
            file.read(reinterpret_cast<char*>(&inner.KeyCount), sizeof(inner.KeyCount));
            file.read(reinterpret_cast<char*>(&inner.Keys), sizeof(inner.Keys));
            file.read(reinterpret_cast<char*>(&inner.ChildPageIds), sizeof(inner.ChildPageIds));
        } break;
        case 2:
        {
            LeafPage_Inner inner{};
            file.read(reinterpret_cast<char*>(&inner.ParentPageID), sizeof(inner.ParentPageID));
            file.read(reinterpret_cast<char*>(&inner.KeyCount), sizeof(inner.KeyCount));
            file.read(reinterpret_cast<char*>(&inner.Keys), sizeof(inner.Keys));
            file.read(reinterpret_cast<char*>(&inner.ChildPages), sizeof(inner.ChildPages));
        } break;
    };

    return p;
};

const Page& Buffer::getPage(ID_Int pageId){
    if (auto search = bufferMap.find(pageId); search != bufferMap.end()){
        return *(search->second); //return page already stored in memory
    } else {
        if (pageList.size() >= BUFFER_SIZE){
            auto lastPageID = pageList.back().getId();
            bufferMap.erase(lastPageID);
            pageList.pop_back();
        }

        pageList.push_front(serializer.getPage(pageId));
        bufferMap.emplace(pageId, pageList.begin());
        return pageList.front();
        
    }
};