/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_DEBT_TO_INCOME_RATIO_HPP__
#define __LC_DEBT_TO_INCOME_RATIO_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class DebtToIncomeRatio : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    DebtToIncomeRatio(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
    {
        static const std::vector<FilterValue>* options = create_range(10 * 100, 35 * 100, 5 * 100);
        Filter::initialize(options, current);
    }

    virtual FilterValue convert(const std::string& raw_data)
    {
        // Convert DTI 19.48 into normalized 1948
        std::string data;
        if (raw_data[raw_data.length() - 1] == '%') {
            data = raw_data.substr(0, raw_data.length() - 1);            
        }
        else {
            data = raw_data;
        }
        return boost::numeric_cast<FilterValue>(strtod(data.c_str(), nullptr) * 100);
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.debt_to_income_ratio <= self.get_value());
    }

    inline bool apply(const LCLoan& loan)
    {
        return (loan.debt_to_income_ratio <= get_value());
    }

    std::string get_string_value() const
    {
        return boost::lexical_cast<std::string>(boost::numeric_cast<double>(get_value()) / 100);
    }

};

};

#endif // __LC_DEBT_TO_INCOME_RATIO_HPP__