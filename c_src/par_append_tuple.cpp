#include "nifpp.h"
#include <functional>
#include <utility>  

class ResultSet
{
    std::unordered_map<int,std::vector<nifpp::TERM>> map_of_result_sets;
    std::unordered_map<int,std::vector<std::pair<int,int>>> current_result_index;
    std::unordered_map<int,std::vector<int>> reverse_result_index;
    std::unordered_map<int,ErlNifEnv*> env_map;
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
       for (int i = 1; i <= init_count; ++i)
           reverse_result_index.emplace(i,empty_vector);
       env = NULL; 
    }
    int get_result_count()
    {
        return result_count;
    }
    int get_total_count()
    {
        return total_count;
    }
    std::unordered_map<int,std::vector<nifpp::TERM>> get_map_of_results_sets()
    {
        return map_of_result_sets;
    }
    std::vector<std::vector<nifpp::TERM>> append_result_set(std::vector<nifpp::TERM> result_set,int uid, ErlNifEnv* env_outer)
    {
        std::vector<std::vector<nifpp::TERM>> final_result_set = {};
        if(result_count < total_count)
        {
            map_of_result_sets.emplace(uid, result_set);
            env_map.emplace(uid, env_outer);
            restructure_current_result_index(env_outer, uid);
            result_count++ ;
            if (result_count==total_count)
            {
                final_result_set = get_final_result_set(env_outer);
            }
        }
        return final_result_set;
    }
    void print_current_result_set()
    {
        for (auto it = current_result_index.begin(); it != current_result_index.end(); ++it)
            printf("%d\n", it->first);

    }
    void restructure_current_result_index(ErlNifEnv* env_outer, int uid)
    {
        int index = 0;
        ErlNifBinary ebin, bin_term;
        
        std::vector<nifpp::TERM> result_set = map_of_result_sets.at(uid);
        
        int arity;
        int hash_val;
        const ERL_NIF_TERM* tuple;
        int size = result_set.size();
        for (int i = 0; i < size; ++i)
        {
            if(enif_get_tuple(env_outer, result_set.at(i), &arity, &tuple)!=1)
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
        const ERL_NIF_TERM* tuple1;
        const ERL_NIF_TERM* tuple2;
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
                ErlNifEnv* env_inner = env_map.at(curr_pair.first);
                if (result_count==2)
                {
                     if(enif_get_tuple(env_inner, map_of_result_sets.at(curr_pair.first).at(curr_pair.second), &arity, &tuple2)!=1)
                        enif_make_badarg(env);
                        printf("%d%d%d%d\n", curr_pair.first, curr_pair.second, arity, comp_arity);
                        nifpp::get_throws(env_inner, tuple2[comp_arity], ebin);
                }
                else
                {
                    if(enif_get_tuple(env_inner, map_of_result_sets.at(curr_pair.first).at(curr_pair.second), &arity, &tuple1)!=1)
                        enif_make_badarg(env);
                        nifpp::get_throws(env_inner, tuple1[comp_arity], ebin);
                }
               
                // if (inner_index==0)
                // {
                //     while(comp_arity != arity && comp_arity < arity)
                //     {
                //         printf("%s\n", "a");
                //         nifpp::get_throws(env_inner, tuple[comp_arity], ebin);
                //         printf("%s\n", "b");
                //         enif_alloc_binary(ebin.size, &bin_term);
                //         memcpy(bin_term.data, ebin.data, ebin.size);
                //         term = nifpp::make(env, bin_term);
                //         array_of_binaries.push_back(term);
                //         comp_arity = comp_arity + 1;
                //     }

                // }
                // else
                // {
                //     comp_arity =  1;
                //     while(comp_arity != arity && comp_arity < arity)
                //     {
                //         nifpp::get_throws(env_inner, tuple[comp_arity], ebin);
                //         enif_alloc_binary(ebin.size, &bin_term);
                //         memcpy(bin_term.data, ebin.data, ebin.size);
                //         term = nifpp::make(env, bin_term);
                //         array_of_binaries.push_back(term);
                //         comp_arity = comp_arity + 1;
                //     }

                // }
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
        std::vector<std::vector<nifpp::TERM>> new_result = (*ptr).append_result_set(input_result_set, uid, env);
        return nifpp::make(env, new_result);
        // return result;
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
            std::vector<std::vector<nifpp::TERM>> new_result = (*ptr).get_final_result_set(env);
            return nifpp::make(env, new_result);
        }
        else
            return enif_make_badarg(env);
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

    }
    catch(...){}
    return enif_make_badarg(env);
}

extern "C" {

static ErlNifFunc nif_funcs[] = {
                                    {"initialize_result_set", 1, initialize_result_set}, 
                                    {"return_result_set", 1, return_result_set},
                                    {"append_result_set", 3, append_result_set},
                                };

ERL_NIF_INIT(par_append_tuple, nif_funcs, load, NULL, NULL, NULL)

}//extern C