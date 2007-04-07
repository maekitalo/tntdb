////////////////////////////////////////////////////////////////////////
// util.h
//

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <iostream>
#include <tntdb/value.h>
#include <tntdb/row.h>

class ValuePrinter
{
    std::ostream& out;
    char delimiter;

  public:
    explicit ValuePrinter(std::ostream& out_, char delimiter_ = '\t')
      : out(out_), delimiter(delimiter_)
      { }
    explicit ValuePrinter(char delimiter_ = '\t')
      : out(std::cout), delimiter(delimiter_)
      { }

    void operator() (const tntdb::Value& v) const
    {
      if (v.isNull())
        out << "null" << delimiter;
      else
        out << v.getString() << delimiter;
    }
};

class RowPrinter
{
    std::ostream& out;
    char delimiter;

  public:
    explicit RowPrinter(std::ostream& out_, char delimiter_ = '\n')
      : out(out_),
        delimiter(delimiter_)
      { }

    explicit RowPrinter(char delimiter_ = '\n')
      : out(std::cout),
        delimiter(delimiter_)
      { }

    void operator() (const tntdb::Row& t) const
    {
      std::for_each(t.begin(), t.end(),
        ValuePrinter(out, '\t'));
      out << delimiter;
    }
};

#endif // UTIL_H

