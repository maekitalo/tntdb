## Date and time support

The syntax of date and time varies between different databases and is
sometimes very confusing. To help here Tntdb offers classes:

    - `tntdb::Date`
    - `tntdb::Time`
    - `tntdb::Datetime`

The classes have just enough functionality to work with dates and times in the
database, a constructor and getters for the components. `tntdb::Time` has milliseconds
precision.

Example to insert and retrieve a date:

```cpp
    auto sel = conn.prepare("select max(last_update) from table");
    tntdb::Date lastUpdate;
    sel.selectValue()
       .get(lastUpdate);
    std::cout << "the last update is " << lastUpdate.getIso() << std::endl;

    auto ins = conn.prepare("insert into table(id, a_date) values (:id, :aDate)");
    ins.set("id", 5)
       .set("aDate", tntdb::Date(2026, 8, 5))
       .execute();
```

This process is similar for `tntdb::Datetime` and `tntdb::Time`.
