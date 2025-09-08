# SimpleDB (C++)
SimpleDB is a small, self-contained DBMS written in C++ with an emphasis on:

* **Storage & buffer management**: on-disk page format and an in-memory buffer pool with LRU eviction.
* **Indexes**: B+ tree index implementation for quick searching and reduced cache misses.
* **Query layer**: lightweight SQL-like parser and interface.
* **Multi-threading**: Support for parallelism in query retrieval and database updates.
