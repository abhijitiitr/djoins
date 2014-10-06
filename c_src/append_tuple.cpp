#include "nifpp.h"
#include <functional>

using std::make_tuple;
using std::ref;

class ResultSet
{
    std::vector<std::vector<std::vector<nifpp::TERM>>> array_of_result_sets;
    std::unordered_map<int,std::vector<int>> current_result_index;
    int count;
    ErlNifEnv* env;
    bool is_first_iteration;
public:
    ResultSet()
    {
       count = 0;
       array_of_result_sets = {};
       current_result_index = {};
       env = NULL; 
       is_first_iteration = true;
    }

    std::vector<std::vector<nifpp::TERM>> append_result_set(std::vector<std::vector<nifpp::TERM>> result_set,ErlNifEnv* env_outer)
    {
        array_of_result_sets.push_back(result_set);
        restructure_current_result_index(env_outer);
        count = count - 1;
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        if(count==0)
        {
            final_result = get_final_result_set();
        }
        return final_result;
    }

    void set_count(int count)
    {
        count = count;
    }

    void restructure_current_result_index(ErlNifEnv* env_outer)
    {
        std::unordered_map<int,std::vector<int>> new_result_index;
        std::vector<std::vector<nifpp::TERM>> last_result_set = array_of_result_sets.back();
        int index = 0;
        for (auto it = last_result_set.begin(); it != last_result_set.end();++it)
        {
            ErlNifBinary binary, bin_term;
            nifpp::get_throws(env, (*it).at(0), binary);
            enif_alloc_binary(binary.size, &bin_term);
            memcpy(bin_term.data, binary.data, binary.size);
            if(is_first_iteration==true)
            {
                std::vector<int> int_list;
                int_list.push_back(index);
                new_result_index.emplace (bin_term.data[0], int_list);
            }
            else
            {
                auto it = current_result_index.find(bin_term.data[0]);
                if(it != current_result_index.end()) 
                {
                    current_result_index.at(bin_term.data[0]).push_back(index);
                    new_result_index.emplace (bin_term.data[0], current_result_index.at(bin_term.data[0]));
                }
            }
            current_result_index = new_result_index;
            index = index + 1;
        }
    }

    std::vector<std::vector<nifpp::TERM>> get_final_result_set()
    {
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        int index = 0; int inner_index = 0;
        for (auto it = current_result_index.begin(); it != current_result_index.end(); ++it)
        {
            inner_index = 0;
            std::vector<nifpp::TERM> appended_rows = {};
            std::vector<int> array_of_indices = it->second;
            for (auto it_inner = array_of_indices.begin(); it_inner != array_of_indices.end() ; ++it_inner)
            {
                std::vector<nifpp::TERM> array_of_binaries = array_of_result_sets.at(index).at(*it_inner);
                if (inner_index==0)
                {
                    appended_rows.insert(appended_rows.end(),array_of_binaries.begin(),array_of_binaries.end());
                }
                else
                {
                    appended_rows.insert(appended_rows.end(),array_of_binaries.begin()+1,array_of_binaries.end());
                }
                inner_index = inner_index + 1;
            }
            final_result.push_back(appended_rows);
            index = index + 1;
        }

        return final_result;
    }

    ~ResultSet();

    /* data */
};
static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    nifpp::register_resource<std::vector<std::vector<std::vector<nifpp::TERM>>>>(env, nullptr, "array_of_result_sets");
    return 0;
}

static ERL_NIF_TERM append_tuple(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    try
    {   std::vector<nifpp::TERM> v;
    	std::vector<nifpp::TERM> mylist1, mylist2;
    	std::vector<nifpp::TERM> output = {};
    	nifpp::get_throws(env, argv[0], mylist1);
    	nifpp::get_throws(env, argv[1], mylist2);
  	 	ErlNifBinary ebin, bin_term;
  	 	const ERL_NIF_TERM* tuple;
  	 	int arity;
        std::vector<std::vector<nifpp::TERM>> array_of_rows;
        nifpp::get_throws(env, argv[3],array_of_rows);
		// nifpp::get_throws(env, argv[0], ebin); // Assigns ebin to the input binary
		// enif_alloc_binary(ebin.size, &bin_term); // Size of new binary
		// memcpy(bin_term.data, ebin.data, ebin.size); // Copying the contents of binary
		for (std::vector<nifpp::TERM>::iterator it = mylist1.begin() ; it != mylist1.end(); ++it)
    	{
    		enif_get_tuple(env, *it, &arity, &tuple);
    		nifpp::get_throws(env, tuple[0], ebin);
    		enif_alloc_binary(ebin.size, &bin_term);
    		memcpy(bin_term.data, ebin.data, ebin.size);
    		output.emplace (output.end(), enif_make_binary(env, &bin_term));
    	}
		mylist1.insert(mylist1.end(),mylist2.begin()+1, mylist2.end());
		return nifpp::make(env,output);
    }
    catch(nifpp::badarg) {}
    catch(...) {}
    return enif_make_badarg(env);
}

// static ERL_NIF_TERM return_binary_copy()
// {

// }
extern "C" {

static ErlNifFunc nif_funcs[] = { {"append_tuple", 2, append_tuple}};

ERL_NIF_INIT(append_tuple, nif_funcs, NULL, NULL, NULL, NULL)

} //extern C



// int arity;
// nifpp::TERM *array;
// vector<int> data;

// enif_get_tuple(env, term, &arity, &array);
// data.resize(arity);
// std::transform(array, array+arity, data.begin(), [&](nifpp::TERM t)
//     {return nifpp::get<int>(env, t);});