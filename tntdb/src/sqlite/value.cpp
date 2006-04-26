////////////////////////////////////////////////////////////////////////
// sqlite/data.cpp
//

#include <tntdb/sqlite/value.h>
#include <sstream>

namespace tntdb
{
  namespace sqlite
  {
    template <typename T> std::string to_string(T v)
    {
      std::ostringstream o;
      o << v;
      return o.str();
    }

    template <typename T> std::pair<T, bool> string_to(const std::string& v)
    {
      T ret;
      std::istringstream in(v);
      in >> ret;
      return std::pair<T, bool>(ret, in);
    }

    void Value::set(bool data)
    {
      null = false;
      this->data = (data ? "1" : "0");
    }

    void Value::set(int data)
    {
      this->data = to_string(data);
      null = false;
    }

    void Value::set(unsigned data)
    {
      this->data = to_string(data);
      null = false;
    }

    void Value::set(long data)
    {
      this->data = to_string(data);
      null = false;
    }

    void Value::set(float data)
    {
      this->data = to_string(data);
      null = false;
    }

    void Value::set(double data)
    {
      this->data = to_string(data);
      null = false;
    }

    void Value::set(char data)
    {
      this->data = data;
      null = false;
    }

    void Value::set(const char* data)
    {
      if (data)
      {
        this->data = data;
        null = false;
      }
      else
        null = true;
    }

    void Value::set(const std::string& data)
    {
      this->data = data;
      null = false;
    }

    bool Value::getBool() const
    {
      return !null
          && data.size() > 0
          &&  data[0] == '1'
           || data[0] == 't'
           || data[0] == 'T';
    }

    int Value::getInt() const
    {
      return string_to<int>(data).first;
    }

    unsigned Value::getUnsigned() const
    {
      return string_to<unsigned>(data).first;
    }

    long Value::getLong() const
    {
      return string_to<long>(data).first;
    }

    float Value::getFloat() const
    {
      return string_to<float>(data).first;
    }

    double Value::getDouble() const
    {
      return string_to<double>(data).first;
    }

    char Value::getChar() const
    {
      return data.size() > 0 ? data[0] : '\0';
    }

  }
}
