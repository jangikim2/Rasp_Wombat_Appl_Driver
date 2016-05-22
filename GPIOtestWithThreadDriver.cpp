#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <cassert>

#include <mutex>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GPIOClass.h"

#include <fcntl.h>
//using namespace std;

void sig_handler(int sig);

bool ctrl_c_pressed = false;
string output_gpio22 = "0";
GPIOClass* gpio4;
GPIOClass* gpio22;
GPIOClass* gpio24;
string inputstate;

std::mutex mu;
int fd;

void task(int i)
{
    char wbuf[4];
    //wbuf[0] = 1;
    //wbuf[1] = 17;
    //write(fd, wbuf, sizeof(wbuf));

        while(1) {
		mu.lock();
		if(ctrl_c_pressed)
                	break;

		if(i == 0)
		{
			//output_gpio22 = "1";
                	//gpio22->setval_gpio("1");
			wbuf[0] = 1;
    			wbuf[1] = 4;
    			write(fd, wbuf, sizeof(wbuf));
		}
		else
		{
                	//gpio22->setval_gpio("0");
			//output_gpio22 = "0";
                        wbuf[0] = 0;
                        wbuf[1] = 4;
                        write(fd, wbuf, sizeof(wbuf));

		}
                //gpio22->setval_gpio(output_gpio22);
                usleep(50000);
		mu.unlock();
                std::cout << "worker : " << i << "\n";
        	//usleep(1000000);
		//mu.unlock();
         //if(ctrl_c_pressed)
		//break;
        }
        cout << "Worker thread exit " << i  <<endl;
}

int main (void)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    //string inputstate;
/* 
    gpio4 = new GPIOClass("4");
    gpio24 = new GPIOClass("24");
    gpio22 = new GPIOClass("22");

    gpio4->export_gpio();
    gpio24->export_gpio();
    gpio22->export_gpio();

    cout << " GPIO pins exported" << endl;

    gpio24->setdir_gpio("in");
    gpio4->setdir_gpio("out");
    gpio22->setdir_gpio("out");

    cout << " Set GPIO pin directions" << endl;

    // vector container stores threads
    //gpio22->setval_gpio("0");


    std::vector<std::thread> workers;
    for (int i = 0; i < 2; i++) {
        auto t = std::thread(&task, i);
        workers.push_back(std::move(t));
    }
    std::cout << "main thread\n";

    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
	//assert(t.joinable());
        //t.join();
        t.detach();
    });
*/

    //int fd;
    fd = open("/dev/io_dev", O_RDWR);
    if(!(fd > 0))
    {
            cout << "Device open is failed!!!"<<endl;
	    return 0;
    }

    char buf[4];
    char wbuf[4];
    wbuf[0] = 1;
    wbuf[1] = 17;
    write(fd, wbuf, sizeof(wbuf));

    std::vector<std::thread> workers;
    for (int i = 0; i < 2; i++) {
        auto t = std::thread(&task, i);
        workers.push_back(std::move(t));
    }
    std::cout << "main thread\n";

    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        //assert(t.joinable());
        //t.join();
        t.detach();
    });

    std::cout << "main thread while\n";

    while(1)
    {

        usleep(1000000);
/*
	if(wbuf[0] == 0)
		wbuf[0] = 1;
	else
	 	wbuf[0] = 0;

        wbuf[1] = 0;
        write(fd, wbuf, sizeof(wbuf));
*/
        //gpio24->getval_gpio(inputstate);
	read(fd, buf, sizeof(buf)); 
/*        cout << "message 0 : " << buf[0] << endl;
        cout << "message 1 : " << buf[1] << endl;
        cout << "message 2 : " << buf[2] << endl;
        cout << "message 3 : " << buf[3] << endl;
*/
        printf("message 0 : %x \n", buf[0]);
        printf("message 1 : %x \n", buf[1]);
        printf("message 2 : %x \n", buf[2]);
        printf("message 3 : %x \n", buf[3]);


        //if(inputstate == "0")
	if(buf[0] == 0xb2)
        {
            cout << "input pin state is Pressed .n Will check input pin state again in 20ms "<<endl;
            usleep(20000);
            cout << "Checking again ....." << endl;
            //gpio24->getval_gpio(inputstate);
            read(fd, buf, sizeof(buf)); 
            //if(inputstate == "0")

            if(buf[0] == 0xb2)
            {
                cout << "input pin state is definitely Pressed. Turning LED ON" <<endl;
                //gpio4->setval_gpio("1");
                wbuf[0] = 1;
	        wbuf[1] = 17;
                write(fd, wbuf, sizeof(wbuf));


                cout << " Waiting until pin is unpressed....." << endl;
                //while (inputstate == "0"){
	        while (buf[0] == 0xb2){

                    //gpio24->getval_gpio(inputstate);
                    read(fd, buf, sizeof(buf)); 
                };
                cout << "pin is unpressed" << endl;

            }
            else
                cout << "input pin state is definitely UnPressed. That was just noise." <<endl;

        }
        //gpio4->setval_gpio("0");

	wbuf[0] = 0;
        wbuf[1] = 17;
	write(fd, wbuf, sizeof(wbuf));

/*
        gpio24->getval_gpio(inputstate);
        cout << "Current input pin state is " << inputstate  <<endl;
        if(inputstate == "0")
        {
           cout << "input pin state is Pressed .n Will check input pin state again in 20ms "<<endl;
           usleep(20000);
                   cout << "Checking again ....." << endl;
                   gpio24->getval_gpio(inputstate);
            if(inputstate == "0")
            {
                cout << "input pin state is definitely Pressed. Turning LED ON" <<endl;
                gpio4->setval_gpio("1");

                cout << " Waiting until pin is unpressed....." << endl;
                while (inputstate == "0"){
                    gpio24->getval_gpio(inputstate);
                };
                cout << "pin is unpressed" << endl;

            }
            else
                cout << "input pin state is definitely UnPressed. That was just noise." <<endl;

        }
        gpio4->setval_gpio("0");
*/

         if(ctrl_c_pressed)
                    {
			/*
			    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
			    {
			        //assert(t.joinable());
			        //t.join();
			        if(t.joinable())
			        {
			                cout << "t.joinable()....." << endl;
			                t.join();
			        }
			    });

                        cout << "Ctrl^C Pressed" << endl;
                        cout << "unexporting pins" << endl;
                        gpio4->unexport_gpio();
                        gpio24->unexport_gpio();
                        cout << "deallocating GPIO Objects" << endl;
                        delete gpio4;
                        gpio4 = 0;
                        delete gpio24;
                        gpio24 =0;
                        delete gpio22;
                        gpio22 = 0;
			*/
			close(fd);
                        break;

                    }


    }
    cout << "Exiting....." << endl;
    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}
 
