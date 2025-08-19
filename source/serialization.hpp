// Table title (64 chars)
// NumColumns Int (1 byte - max 255)
// Columnn names (NumColumns * 64 chars - 256 * 64 bytes)
// Types (NumColumns * 1byte - 256 bytes)


// Page
// ________________________________
// NumRows (long)
// Rows

// Indexes
// ________________________________


// struct MetaPage{}
// struct RecordPage{}
// struct NodePage{}
// struct RootPage{}

// class Serializer{
// public:
//     getPage(pageId)

//     writePage(pageId, page)
// }

// class BufferManager{
//     int numPages;
//     map bufferPool; // pageid's to acc pages

// public:
//     fetchPage()
//     def unpin_page(self, page_id, is_dirty=False):
//         """
//         Tell buffer manager we’re done using this page.
//         - Decrease pin count.
//         - Mark as dirty if modified (so it must be written back later).
//         """

//     def flush_page(self, page_id):
//         """
//         Force write a dirty page back to disk.
//         """

//     def flush_all(self):
//         """
//         Write all dirty pages back to disk.
//         """
// }

// class BPTree{
// public:
//     getIndexes(condition)
//     addIndex()
// }

// class SequentialSearch{
// public:
//     getIndexes(condition)
// }

