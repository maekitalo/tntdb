# Custom data types

When working with prepared statements and fetching values from the database
Tntdb supports practically all scalar types in C++. But sometimes we want to
store some special values in a database field. We can convert the special value
to and from existing types when inserting or extracting the value. Tntdb
offers a convenient way to add such support.

For fetching and sending values Tntdb uses special operators, which can be
implemented for custom data types.

## The problem

As a very simple example we want to store an enum like:

```cpp
    enum class Operation : int { PLUS, MINUS, MULTIPLY, DIVIDE }
```

To insert an `Oerpation` object into the database we write:

```cpp
    Operation op = Operation::PLUS;

    auto ins = conn.prepare("insert into table (id, op) values (:id, :op)");
    ins.set("id", 5)
       .set("op", static_cast<int>(op))
       .execute();
```

To retrieve an `Operation` object from the database we write:

```cpp
    Operation op;

    auto sel = conn.prepare("select op from table where id = :id");
    int intValue;
    sel.set("id", 5)
       .selectValue()
       .get(intValue);
    op = static_cast<Operation>(intValue);
```

This works, but static casting manually before inserting or after fetching is
a little inconvenient. To make it more natural we can define operators for that.

## The solution: setting values

To allow a custom type to be used as a statement parameter, we implement the operator <<.
The operator receives a reference to a `tntdb::Hostvar` and a const reference to the
object being stored.

`tntdb::Hostvar` provides set methods for all types supported by tntdb. The custom type
can therefore be converted to one of these supported types and stored using `set`:

```cpp
    void operator<< (tntdb::Hostvar& hv, const Operation& op)
    {
        hv.set(static_cast<int>(op));
    }
```

Now we can write:

```cpp
    Operation op = Operation::PLUS;

    auto ins = conn.prepare("insert into table (id, op) values (:id, :op)");
    ins.set("id", 5)
       .set("op", op)
       .execute();
```

## The solution: retrieving values

To retrieve a value we implement an operator>> which has a `const tntbd::Value&`
as a left hand operand and a non const reference to our type as a right hand operand.
It must return a bool to indicate whether the value was not null:

```cpp
    bool operator>> (const tntdb::Value& v, Operation& op)
    {
        if (v.isNull())
            return false;
        int intValue;
        v.get(intValue);
        op = static_cast<Operaton>(intValue);
        return true;
    }
```

Now we can write:

```cpp
    Operation op;

    auto sel = conn.prepare("select op from table where id = :id");
    sel.set("id", 5)
       .selectValue()
       .get(op);
```

## A little more complex example

We define a rgb color struct containting 3 values:

```cpp
    struct Rgb 
    {
        unsigned short red;
        unsigned short green;
        unsigned short blue;
    };

    void operator<< (tntdb::Hostvar& hv, const Rgb& rgb)
    {
        std::string str = std::to_string(rgb.red)
                        + std::to_string(rgb.green)
                        + std::to_string(rgb.blue);
        hv.set(str);
    }

    bool operator>> (const tntdb::Value& v, Rgb& rgb)
    {
        std::string str;
        if (!v.get(str))
            return false;
        std::sscanf(rgb.c_str(), "%hu-%hu-%hu", &rgb.red, &rgb.green, &rgb.blue);
        return true;
    }
```

Now we can easily work with that:

```cpp
    Rgb red{255, 0, 0};
    conn.prepare("insert into colortable (id, color) values (:id, :color))
        .set("id", 7)
        .set("color", red)
        .execute();

    Rgb theColor;
    conn.prepare("select color from colortable where id = :id")
        .set("id", 7)
        .selectValue()
        .get(theColor);
```
