# Date and time support

Date and time syntax differs between database systems. Tntdb therefore provides
database-independent classes for date and time values.

The classes provide the functionality needed for database access, including
constructors and getters for their components. `tntdb::Time` supports
millisecond precision. The classes do not validate whether the supplied
component values form a valid date or time.

## Example

```cpp
auto sel = conn.prepare("select max(last_update) from table");

tntdb::Date lastUpdate;
sel.selectValue()
   .get(lastUpdate);

std::cout << "the last update is "
          << lastUpdate.getIso()
          << std::endl;
```

A date can be passed to a prepared statement just like other supported values:

```cpp
auto ins = conn.prepare(
    "insert into table (id, a_date) values (:id, :aDate)");

ins.set("id", 5)
   .set("aDate", tntdb::Date(2026, 8, 5))
   .execute();
```

The same approach applies to `tntdb::Datetime` and `tntdb::Time`.
