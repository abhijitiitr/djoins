#include "nifpp.h"
#include <functional>

using std::make_tuple;
using std::ref;

extern "C" {
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

static ErlNifFunc nif_funcs[] = { {"append_tuple", 2, append_tuple} };

ERL_NIF_INIT(append_tuple, nif_funcs, NULL, NULL, NULL, NULL)

} //extern C
