#include <vector>
#include <thread>
#include <boost/program_options.hpp>
#include "LCBT.hpp"
#include "LCGA.hpp"

using namespace lc;
using namespace std;

int lcmain(int argc, char* argv[])
{
    // Declare the supported options.
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message")
        ("verbose,v", "Set verbosity level")
        ("version,V", boost::program_options::value<string>()->default_value("0.0"), "Program version")
        ("grades,g", boost::program_options::value<string>()->default_value("ABCDEF"), "String with the allowed credit grades")
        ("seed,s", boost::program_options::value<unsigned>()->default_value(100), "Random Number Generator Seed")
        ("data,d", boost::program_options::value<string>()->default_value("https://www.lendingclub.com/fileDownload.action?file=LoanStatsNew.csv&type=gen"), "Download path for the notes data file")
        ("stats,l", boost::program_options::value<string>()->default_value("LoanStatsNew.csv"), "Input Loan Stats CSV file")
        ("csvresults,c", boost::program_options::value<string>()->default_value("lc_best.csv"), "Output best results CSV file")
        ("population_size,p", boost::program_options::value<unsigned>()->default_value(512), "population size")
        ("iterations,i", boost::program_options::value<unsigned>()->default_value(4096), "how many Genetic Algorithm iterations to perform")
        ("elite_rate,e", boost::program_options::value<double>()->default_value(0.05), "elite rate")
        ("mutation_rate,m", boost::program_options::value<double>()->default_value(0.05), "mutation rate")
        ("check,k", boost::program_options::value<bool>()->default_value(false), "checking mode: open the CSV results file and filter the loans into a separate file")
        ("checkresults,r", boost::program_options::value<string>()->default_value("LoanStatsNewFiltered.csv"), "Output best results CSV file")
        ("zmq,z", boost::program_options::value<bool>()->default_value(false), "Use zmq libraries for multi-processing")
        ("fitness_sort_size,f", boost::program_options::value<unsigned>()->default_value(1000), "number of loans to limit the fitness sort size, the larger the longer and more optimal solution")
        ("young_loans_in_days,y", boost::program_options::value<unsigned>()->default_value(3*30), "filter young loans if they are younger than specified number of days")
        ("workers,w", boost::program_options::value<unsigned>()->default_value(std::thread::hardware_concurrency()), "number of workers defaults to the number of cpu cores")
        ("work_batch,b", boost::program_options::value<unsigned>()->default_value(75), "size of work batch size to give to each worker")
    ;

    Arguments args;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), args);
    boost::program_options::notify(args);

    if (args.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    srand(args["seed"].as<unsigned>());

    std::vector<LCLoan::LoanType> conversion_filters;
    conversion_filters.push_back(LCLoan::ACC_OPEN_PAST_24MTHS);
    conversion_filters.push_back(LCLoan::FUNDED_AMNT);
    conversion_filters.push_back(LCLoan::ANNUAL_INCOME);
    conversion_filters.push_back(LCLoan::GRADE);
    conversion_filters.push_back(LCLoan::DEBT_TO_INCOME_RATIO);
    conversion_filters.push_back(LCLoan::DELINQ_2YRS);
    conversion_filters.push_back(LCLoan::EARLIEST_CREDIT_LINE);
    conversion_filters.push_back(LCLoan::EMP_LENGTH);
    conversion_filters.push_back(LCLoan::HOME_OWNERSHIP);
    conversion_filters.push_back(LCLoan::INCOME_VALIDATED);
    conversion_filters.push_back(LCLoan::INQ_LAST_6MTHS);
    conversion_filters.push_back(LCLoan::PURPOSE);
    conversion_filters.push_back(LCLoan::MTHS_SINCE_LAST_DELINQ);
    conversion_filters.push_back(LCLoan::PUB_REC);
    conversion_filters.push_back(LCLoan::REVOL_UTILIZATION);
    conversion_filters.push_back(LCLoan::ADDR_STATE);
    conversion_filters.push_back(LCLoan::TOTAL_ACC);
    conversion_filters.push_back(LCLoan::DESC_WORD_COUNT);

    LCBT lcbt(conversion_filters, args, -1);
    lcbt.initialize();

    std::vector<LCLoan::LoanType> backtest_filters = conversion_filters;

    GATest ga_test(backtest_filters, lcbt, args);
    ga_test.run();

    return 0;
}
