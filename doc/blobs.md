# Binary large objects

Binary large objects (BLOBs) can be large and may consume significant amounts
of memory when retrieved. Tntdb provides `tntdb::Blob` for accessing binary
data while avoiding loading the complete value into memory when the database
driver supports streaming access.

To read a BLOB as a stream, use `tntdb::BlobIStream`. It derives from
`std::istream`, so standard C++ stream operations can be used to process the
data incrementally.

## Example

```cpp
void foo(tntdb::Connection conn)
{
    auto sel = conn.prepare("select blob from foo");

    for (auto row : sel)
    {
        tntdb::Blob blob;
        row.get(blob);

        tntdb::BlobIStream in(blob);

        // Read and output the first 120 bytes.
        char buffer[120];
        in.read(buffer, sizeof(buffer));
        std::cout.write(buffer, in.gcount());
    }
}
```

The example iterates over the selected rows, retrieves the BLOB from each row,
and creates a `tntdb::BlobIStream` for it. It then reads up to 120 bytes and
writes the bytes that were actually read to `std::cout`.
