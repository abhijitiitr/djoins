#include "nifpp.h"
#include <functional>
#include <utility>  

class ResultSet
{
    std::unordered_map<int,std::vector<nifpp::TERM>> map_of_result_sets;
    std::unordered_map<int,std::vector<std::pair<int,int>>> current_result_index;
    std::unordered_map<int,std::vector<int>> reverse_result_index;
    int result_count;
    int total_count;
    ErlNifEnv* env;
public:
    ResultSet(int init_count)
    {
       total_count = init_count;
       result_count = 0;
       map_of_result_sets = {};
       current_result_index = {};
       std::vector<int> empty_vector = {};
       for (int i = 0; i < init_count; ++i)
       {
           reverse_result_index.emplace(i,empty_vector);
       }
       env = NULL; 
    }
    std::vector<std::vector<nifpp::TERM>> append_result_set(std::vector<nifpp::TERM> result_set,int uid, ErlNifEnv* env_outer)
    {
        map_of_result_sets.emplace(uid, result_set);
        restructure_current_result_index(env_outer, &result_set, uid);
        result_count++ ;
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        if(result_count==total_count)
        {
            final_result = get_final_result_set(env_outer);
        }
        return final_result;
    }
    void print_current_result_set()
    {
        for (auto it = current_result_index.begin(); it != current_result_index.end(); ++it)
        {
            printf("%d\n", it->first);
        }

    }
    void restructure_current_result_index(ErlNifEnv* env_outer, std::vector<nifpp::TERM>* result_set, int uid)
    {
        int index = 0;
        ErlNifBinary ebin, bin_term;
        int arity;
        int hash_val;
        const ERL_NIF_TERM* tuple;
        for (auto it = (*result_set).begin(); it != (*result_set).end(); ++it)
        {
            if(enif_get_tuple(env_outer, *it, &arity, &tuple)!=1)
                enif_make_badarg(env_outer);
            nifpp::get_throws(env_outer, tuple[0], ebin);
            enif_alloc_binary(ebin.size, &bin_term);
            memcpy(bin_term.data, ebin.data, ebin.size);
            hash_val = 0 ;
            for(int i=0; i<bin_term.size; ++i){
               hash_val = (10*hash_val) + bin_term.data[i];
            }
            std::pair<int, int> index_addr = std::make_pair(uid, index);
            std::vector<std::pair<int,int>> init_vector = {};
            init_vector.push_back(index_addr);
            auto emp_res = current_result_index.emplace (hash_val, init_vector);
            if(emp_res.second==false)
                current_result_index.at(hash_val).push_back(index_addr);
            int current_size = current_result_index.at(hash_val).size();
            reverse_result_index.at(current_size).push_back(hash_val);
            index = index + 1;
        }
    }

    std::vector<std::vector<nifpp::TERM>> get_final_result_set(ErlNifEnv* env)
    {
        std::vector<std::vector<nifpp::TERM>> final_result = {};
        int inner_index = 0;
        int arity; int comp_arity = 0;
        const ERL_NIF_TERM* tuple;
        ErlNifBinary ebin, bin_term;
        nifpp::TERM term;
        for (auto it_outer = reverse_result_index.at(total_count).begin(); it_outer != reverse_result_index.at(total_count).end(); ++it_outer)
        {
            std::vector<nifpp::TERM> appended_rows = {};
            inner_index = 0;
            for (auto it_inner = current_result_index.at(*it_outer).begin(); it_inner != current_result_index.at(*it_outer).end() ; ++it_inner)
            {
                comp_arity = 0;
                std::vector<nifpp::TERM> array_of_binaries = {};
                std::pair<int,int> curr_pair = (*it_inner);
                if(enif_get_tuple(env, map_of_result_sets.at(curr_pair.first).at(curr_pair.second), &arity, &tuple)!=1)
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
        int uid;
        nifpp::get(env, argv[0], ptr);
        nifpp::get(env, argv[1], input_result_set);
        nifpp::get(env, argv[2], uid);
        result_set = (*ptr).append_result_set(input_result_set, uid, env);
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
        if(nifpp::get(env, argv[0], ptr)==1)
        {
            // printf("%d\n", (*ptr).get_initial_count());
            (*ptr) = NULL;
        }
        else
        {
            return enif_make_badarg(env);
        }
        // enif_release_resource(&ptr);
        return nifpp::make(env, 1);
    }
    catch(...) {}
    return enif_make_badarg(env);
}

static ERL_NIF_TERM nullify_result_set(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    try
    {
        ERL_NIF_TERM result;
        nifpp::resource_ptr<ResultSet> ptr;
        std::vector<std::vector<nifpp::TERM>> result_set;
        std::vector<nifpp::TERM> input_result_set;
        nifpp::get(env, argv[0], ptr);
        // (*ptr) = NULL;
        // free(ptr);
        return nifpp::make(env, 1);
    }
    catch(...){}
    return enif_make_badarg(env);
}

extern "C" {

static ErlNifFunc nif_funcs[] = {
                                    {"initialize_result_set", 1, initialize_result_set}, 
                                    {"return_result_set", 1, return_result_set},
                                    {"append_result_set", 2, append_result_set},
                                };

ERL_NIF_INIT(par_append_tuple, nif_funcs, load, NULL, NULL, NULL)

}//extern C