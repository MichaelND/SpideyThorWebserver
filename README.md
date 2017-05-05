Project 02 - README
===================
Activity 3: Experiments
1. Average Latency
	Single Connection
		Directory Listings - 
		Static Files -
		CGI Scripts -
	Forking Mode
		Directry Listings -
		Static Files - 
		CGI Scripts -

2. Average Throughput
	Single Connection
		Small Static Files (1 KB)
		Medium Static Files (1 MB)
		Large Static Files (1 GB)
	Forking Mode
		Small Static Files (1 KB)
		Medium Static Files (1 MB)
		Large Static Files (1 GB)


Members
-------
- Michael Wang (mwang6@nd.edu)
- Philip Baumann (pbaumann@nd.edu)

Summary
-------
For this project, students were tasked with setting up a web server. This 
web server can serve single requests as well as forking requests in case 
of multiple requests at the same time. As for what worked and what did 
not, we obviously had some debugging, but we had very few issues with 
working togeter. Given we prefer meeting to work and both prefer working 
early, we accomplished everything required for this lab. 
In general, work was split evenly for this project. Most often, Michael 
and I met to work together, swapping who was typing every few hours or 
so. This way, two minds are always working to produce the best code 
possibe. Other than this, I am writing the report and did a majority of 
the debugging. Michael, on the other hand, did a majority of Thor and 
started several of the programs. 
 
Latency
-------

Throughput
----------

Analysis
--------

Conclusion
----------
As a whole, we've learned a lot about web servers, socketing, forking, and 
requests. From the experiments, we learned the value of forking web 
servers that are capable of accepting multiple requests at any point. This 
greatly improves the overall speed of the server. Additionally, from this 
lab, we learned a lot about HTML formatting and how difficult it is to 
produce well-formatted websites. 

Contributions
-------------
forking.c - Michael and Phil
handler.c - Michael and Phil
Makefile - Michael 
README.md - Phil
request.c - Michael and Phil
single.c - Michael and Phil
socket.c - Michael and Phil
spidey.c - Michael and Phil
thor.py - Michael and Phil
utils.c - Michael and Phil
