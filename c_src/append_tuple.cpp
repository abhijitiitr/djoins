#include "nifpp.h"
#include <functional>


class ResultSet
{
    std::vector<std::vector<nifpp::TERM>> array_of_result_sets;
    std::unordered_map<int,std::vector<int>> current_result_index;
    int count;
    int initial_count;
    ErlNifEnv* env;
    bool is_first_iteration;
public:
    ResultSet(int init_count)
    {
       count = init_count;
       initial_count = init_count;
       array_of_result_sets = {};
       current_result_index = {};
       env = NULL; 
       is_first_iteration = true;
    }

    std::vector<std::vector<nifpp::TERM>> append_result_set(std::vector<nifpp::TERM> result_set,ErlNifEnv* env_outer)
    {
        array_of_result_sets.push_back(result_set);
        restructure_current_result_index(env_outer);
        count = count - 1;
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        if(count==0)
        {
            final_result = get_final_result_set(env_outer);
        }
        return final_result;
    }
    void print_current_result_set()
    {
        for (auto it = current_result_index.begin(); it != current_result_index.end();++it)
        {
            printf("%d\n", it->first);
        }

    }

    int get_count()
    {
        return count;
    }

    std::unordered_map<int,std::vector<int>> get_current_result_index()
    {
        return current_result_index;
    }

    void restructure_current_result_index(ErlNifEnv* env_outer)
    {
        std::unordered_map<int,std::vector<int>> new_result_index;
        std::vector<nifpp::TERM> last_result_set = array_of_result_sets.back();
        int index = 0;
        ErlNifBinary ebin, bin_term;
        int arity;
        int hash_val;
        const ERL_NIF_TERM* tuple;
        for (auto it = last_result_set.begin(); it != last_result_set.end();++it)
        {
            if(enif_get_tuple(env_outer, *it, &arity, &tuple)!=1)
                enif_make_badarg(env_outer);
            nifpp::get_throws(env_outer, tuple[0], ebin);
            enif_alloc_binary(ebin.size, &bin_term);
            memcpy(bin_term.data, ebin.data, ebin.size);
            hash_val = 0;
            for(int i=0; i<bin_term.size; ++i){
               hash_val = (10*hash_val) + bin_term.data[i];
            }
            if(count==initial_count)
            {
                std::vector<int> int_list;
                int_list.push_back(index);
                new_result_index.emplace (hash_val, int_list);
            }
            else
            {
                auto it_inner = current_result_index.find(hash_val);
                if(it_inner != current_result_index.end()) 
                {
                    current_result_index.at(hash_val).push_back(index);
                    new_result_index.emplace (hash_val, current_result_index.at(hash_val));
                }
            }
            index = index + 1;
        }
        current_result_index = new_result_index;
    }

    std::vector<std::vector<nifpp::TERM>> get_final_result_set(ErlNifEnv* env)
    {
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        int index = 0; int inner_index = 0;
        int arity; int comp_arity = 0;
        const ERL_NIF_TERM* tuple;
        ErlNifBinary ebin, bin_term;
        nifpp::TERM term;
        for (auto it = current_result_index.begin(); it != current_result_index.end(); ++it)
        {
            inner_index = 0;
            std::vector<nifpp::TERM> appended_rows = {};
            std::vector<int> array_of_indices = it->second;
            for (auto it_inner = array_of_indices.begin(); it_inner != array_of_indices.end() ; ++it_inner)
            {
                comp_arity = 0;
                std::vector<nifpp::TERM> array_of_binaries = {};
                if(enif_get_tuple(env, array_of_result_sets.at(inner_index).at(*it_inner), &arity, &tuple)!=1)
                    enif_make_badarg(env);
                
                if (inner_index==0)
                {
                    while(comp_arity != arity)
                    {
                        nifpp::get_throws(env, tuple[comp_arity], ebin);
                        enif_alloc_binary(ebin.size, &bin_term);
                        memcpy(bin_term.data, ebin.data, ebin.size);
                        term = nifpp::make(env, bin_term);
                        array_of_binaries.push_back(term);
                        comp_arity = comp_arity + 1;
                    }
                }
                else
                {
                    comp_arity =  1;
                    while(comp_arity != arity)
                    {
                        nifpp::get_throws(env, tuple[comp_arity], ebin);
                        enif_alloc_binary(ebin.size, &bin_term);
                        memcpy(bin_term.data, ebin.data, ebin.size);
                        term = nifpp::make(env, bin_term);
                        array_of_binaries.push_back(term);
                        comp_arity = comp_arity + 1;
                    }
                }
                appended_rows.insert(appended_rows.end(),array_of_binaries.begin(),array_of_binaries.end());

                inner_index = inner_index + 1;
            }
            final_result.push_back(appended_rows);
            index = index + 1;
        }

        return final_result;
    }

    // ~ResultSet();

};
static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    nifpp::register_resource<ResultSet>(env, nullptr, "ResultSet");
    return 0;
}

static ERL_NIF_TERM initialize_result_set(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    try{
        int count;
        nifpp::get(env, argv[0], count);
        ERL_NIF_TERM result;
        auto map_ptr = nifpp::construct_resource<ResultSet>(count);
        result = nifpp::make(env, map_ptr);
        return result;
    }
    catch(nifpp::badarg) {}
    catch(...) {}
    return enif_make_badarg(env);
}

static ERL_NIF_TERM append_result_set(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    try
    {
        ERL_NIF_TERM result;
        nifpp::resource_ptr<ResultSet> ptr;
        std::vector<std::vector<nifpp::TERM>> result_set;
        std::vector<nifpp::TERM> input_result_set;
        nifpp::get(env, argv[0], ptr);
        nifpp::get(env, argv[1], input_result_set);
        result_set = (*ptr).append_result_set(input_result_set, env);
        result = nifpp::make(env, result_set);
        return result;
    }
    catch(...) {}
    return enif_make_badarg(env);
}

static ERL_NIF_TERM return_result_set(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    try
    {
        nifpp::resource_ptr<ResultSet> ptr;
        nifpp::TERM term;
        nifpp::get(env, argv[0], ptr);
        // enif_release_resource(&ptr);
        return nifpp::make(env, 1);
    }
    catch(...) {}
    return enif_make_badarg(env);
}
// static ERL_NIF_TERM return_binary_copy()
// {

// }
extern "C" {

static ErlNifFunc nif_funcs[] = {
                                    {"initialize_result_set", 1, initialize_result_set}, 
                                    {"return_result_set", 1, return_result_set},
                                    {"append_result_set", 2, append_result_set},
                                };

ERL_NIF_INIT(append_tuple, nif_funcs, load, NULL, NULL, NULL)

} //extern C

// int arity;
// nifpp::TERM *array;
// vector<int> data;

// enif_get_tuple(env, term, &arity, &array);
// data.resize(arity);
// std::transform(array, array+arity, data.begin(), [&](nifpp::TERM t)
//     {return nifpp::get<int>(env, t);});