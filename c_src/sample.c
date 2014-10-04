std::vector<nifpp::TERM> mylist1, mylist2;
    	// std::tuple<nifpp::TERM> tpl;
    	// int arity, show;
    	// nifpp::get_throws(env, argv[0], mylist);
    	// auto bar = std::make_tuple ("test", 3.1, 14, 'y',2,2,2,2,2,2,2,22,2,2);
    	nifpp::get_throws(env, argv[0], mylist1);
    	nifpp::get_throws(env, argv[1], mylist2);
    	// enif_get_int(env, tuple[2], &show);
    	// printf("%d\n", show);
		// nifpp::get_throws(env, argv[0], v); // Assigns ebin to the input binary
		// std::deque<int> mydeque;
		// mydeque.push_back(44);
		mylist1.insert(mylist1.end(),mylist2.begin()+1, mylist2.end());
    	// binary bin_term = new binary(ebin.size);
		// enif_alloc_binary(ebin.size, &bin_term); // Size of new binary
		// memcpy(bin_term.data, ebin.data, ebin.size); // Copying the contents of binary
		// return enif_make_binary(env, &bin_term);

		return nifpp::make(env,mylist1);
		// return enif_make_tuple_from_array(env,)