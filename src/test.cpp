// test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <functional>

#include "c11log/logger.h"
#include "c11log/sinks/async_sink.h"
#include "c11log/sinks/file_sinks.h"
#include "c11log/sinks/stdout_sinks.h"

#include "utils.h"


std::atomic<uint64_t> push_count, pop_count;
std::atomic<bool> active;



using std::string;
using std::chrono::seconds;
using Q = c11log::details::blocking_queue<string>;

void pusher(Q* q)
{
	auto &logger = c11log::get_logger("async");
	while(active)
	{
		logger.info()<<"Hello logger!";
		++push_count;		
	}

		
}


void testq(int size, int pushers, int poppers)
{

	active = true;			
	Q q{static_cast<Q::size_type>(size)};		
	
	/*
	for(int i = 0; i < poppers; i++)
		testq(qsize, pushers, poppers);
	*/	
	for(int i = 0; i < pushers; i++)
		new std::thread(std::bind(pusher, &q));
		
	while(active)
	{
		using std::endl;
		using std::cout;
		using utils::format;
		
		push_count = 0;
		pop_count = 0;
		std::this_thread::sleep_for(seconds(1));
		cout << "Pushes/sec =\t" << format(push_count.load()) << endl;
		//cout << "Pops/sec =\t" << format(pop_count.load()) << endl << endl;
		//cout << "Total/sec =\t" << format(push_count+pop_count) << endl;
		cout << "Queue size =\t" << format(q.size()) << endl;
		cout << "---------------------------------------------------------------------" << endl;		
	}		
}


int main(int argc, char* argv[])
{

	if(argc !=4)
	{
		std::cerr << "Usage: " << argv[0] << " qsize, pushers, poppers" << std::endl;
		return 0;
	}
	int qsize = atoi(argv[1]);
	int pushers = atoi(argv[2]);
	int poppers = atoi(argv[3]);
	
	//testq(qsize, pushers, poppers);
	
	
	using namespace std::chrono;
	
								
    auto null_sink = std::make_shared<c11log::sinks::null_sink>();
    auto stdout_sink = std::make_shared<c11log::sinks::stdout_sink>();
    auto async = std::make_shared<c11log::sinks::async_sink>(1000);
    auto fsink = std::make_shared<c11log::sinks::rotating_file_sink>("newlog", "txt", 1024*1024*10 , 2);
    //auto fsink = std::make_shared<c11log::sinks::daily_file_sink>("daily", "txt");
	       
	async->add_sink(fsink);	
	auto &logger = c11log::get_logger("async");
	logger.add_sink(async);
	    
    testq(qsize, pushers, poppers);
    
    
}


