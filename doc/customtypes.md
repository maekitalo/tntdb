# Custom data types

When working with prepared statements and fetching values from the database
Tntdb supports practically all scalar types in C++. But sometimes we want to
store some special values in a database field. We can convert the special value
to and from existing types always when we insert or extract the value. Tntdb
offers a convenient way to add such support.

For fetching and sending values Tntdb uses special operators, which can be
implemented for own data types.

## The problem

As a very simple example we want to store a enum like that:

```cpp
    enum class Operation : int { PLUS, MINUS, MULTIPLY, DEVIDE }
```

To insert into the database we can write:

```cpp
    Operation op = Operation::PLUS;

    auto ins = conn.prepare("insert into table (id, op) values (:id, :op)");
    ins.set("id", 5)
       .set("op", static_cast<int>(op))
       .execute();
```

And retrieve from the database:

```cpp
    Operation op;

    auto sel = conn.prepare("select op from table where id = :id");
    int intValue;
    sel.set("id", 5)
       .selectValue()
       .get(intValue);
    op = static_cast<Operation>(intValue);
```

This is a little inconvenient. To make it more natural we can define operators
for that.

## The solution: setting values

To set a variable to the value we need a operator<<, which takes a reference to
a helper class called `tntdb::Hostvar` and a const reference to our object. The
`tntdb::Hostvar` has as `tntdb::Statement` a set method for all our known
types.

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

To retrieve a value we need to implement a operator>> which has as a left hand operand
a `const tntbd::Value&` and a non const reference to our type as a right hand operand.
It must return a bool to indicate, whether the value was not null:

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

We define a rgb color struct:

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
