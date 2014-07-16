
#include "timestep.hpp"


timestep::timestep(const timestep& src)
{
   
    _itrs = itr_map(src._itrs);
    _date_itr = date_variable::iterator(src._date_itr);
}

timestep::timestep()
{
   
}

timestep::~timestep()
{

}

std::string timestep::to_string()
{
    std::string s;

    for (timestep::itr_map::const_iterator itr = _itrs.begin(); itr != _itrs.end(); itr++)
    {
        s += boost::lexical_cast<std::string>(*(itr->second)) + std::string("\t");
    }

    return s;
}

int timestep::month()
{
    return _date_itr->date().month();
}


int timestep::day()
{
    return _date_itr->date().day();

}

int timestep::year()
{
    return _date_itr->date().year();
}

boost::gregorian::date timestep::get_gregorian()
{
    boost::gregorian::date date;

    date = boost::gregorian::from_string(boost::lexical_cast<std::string>(_date_itr->date()));
    return date;

}

boost::posix_time::ptime timestep::get_posix()
{
    return *_date_itr;
}

double timestep::get(std::string varName)
{
    itr_map::const_accessor a;
    if (!_itrs.find(a, varName))
        BOOST_THROW_EXCEPTION( forcing_lookup_error() << errstr_info("Variable " + varName + " does not exist."));


    double out = a->second[0];

    return out;
}

void timestep::set(std::string varName, double value)
{
    itr_map::accessor a;
    if (!_itrs.find(a, varName))
    {
        BOOST_THROW_EXCEPTION( forcing_lookup_error() << errstr_info("Variable " + varName + " does not exist.")); 
    }

    a->second[0] = value;

}

timestep::variable_vec::iterator timestep::get_itr(std::string varName)
{
    itr_map::const_accessor a;
    if (!_itrs.find(a, varName))
        BOOST_THROW_EXCEPTION( forcing_lookup_error() << errstr_info("Variable " + varName + " does not exist."));


    return a->second;

}