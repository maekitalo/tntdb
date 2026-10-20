# Custom data types

Prepared statements and database values support the common scalar C++ types
directly. Applications sometimes need to store their own types in a single
database field. Tntdb allows custom conversions to be added so such types can
be passed to statements and read from results naturally.

## The problem

Consider an enum stored as an integer:

```cpp
enum class Operation : int
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE
};
```

Without a custom conversion, inserting it requires an explicit cast:

```cpp
Operation op = Operation::PLUS;

auto ins = conn.prepare(
    "insert into table (id, op) values (:id, :op)");

ins.set("id", 5)
   .set("op", static_cast<int>(op))
   .execute();
```

Retrieving it requires the reverse conversion:

```cpp
Operation op;
int intValue;

auto sel = conn.prepare("select op from table where id = :id");
sel.set("id", 5)
   .selectValue()
   .get(intValue);

op = static_cast<Operation>(intValue);
```

Tntdb allows these conversions to be expressed as operators.

## Setting custom values

To pass a custom type as a statement parameter, define `operator<<` for
`tntdb::Hostvar`. The custom type is converted to one of the types already
supported by Tntdb.

```cpp
void operator<<(tntdb::Hostvar& hv, const Operation& op)
{
    hv.set(static_cast<int>(op));
}
```

The custom type can then be used directly:

```cpp
Operation op = Operation::PLUS;

auto ins = conn.prepare(
    "insert into table (id, op) values (:id, :op)");

ins.set("id", 5)
   .set("op", op)
   .execute();
```

## Retrieving custom values

To retrieve a custom type, define `operator>>` with a `const tntdb::Value&` as
the left operand and a reference to the custom type as the right operand. The
operator returns `false` for SQL `NULL` and `true` when a value was converted.

```cpp
bool operator>>(const tntdb::Value& value, Operation& op)
{
    if (value.isNull())
        return false;

    int intValue;
    value.get(intValue);
    op = static_cast<Operation>(intValue);
    return true;
}
```

The custom type can now be read directly:

```cpp
Operation op;

auto sel = conn.prepare("select op from table where id = :id");
sel.set("id", 5)
   .selectValue()
   .get(op);
```

## A more complex example

The same mechanism can serialize a small structure into a single database value:

```cpp
struct Rgb
{
    unsigned short red;
    unsigned short green;
    unsigned short blue;
};

void operator<<(tntdb::Hostvar& hv, const Rgb& rgb)
{
    const std::string str = std::to_string(rgb.red)
                          + '-'
                          + std::to_string(rgb.green)
                          + '-'
                          + std::to_string(rgb.blue);
    hv.set(str);
}

bool operator>>(const tntdb::Value& value, Rgb& rgb)
{
    std::string str;
    if (!value.get(str))
        return false;

    std::sscanf(str.c_str(), "%hu-%hu-%hu",
                &rgb.red, &rgb.green, &rgb.blue);
    return true;
}
```

The type can then be used with statements like any other supported value:

```cpp
Rgb red{255, 0, 0};

conn.prepare("insert into colortable (id, color) values (:id, :color)")
    .set("id", 7)
    .set("color", red)
    .execute();

Rgb theColor;

conn.prepare("select color from colortable where id = :id")
    .set("id", 7)
    .selectValue()
    .get(theColor);
```
