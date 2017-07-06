#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
    void startTestTask(int iters);
    void qtest_block_receive(void);
    void qtest_nonblock_receive(void);
    void qtest_nonblock_send(void);
    void qtest_block_send(void);
    void qtest_performance(void);
    void qtest_concurrent(void);
    void intervalTimerTest(void);
    void fileSystemTest(void);
    void validateFileTest(void);
}

int main(int argc, char* argv[]) {
    
        if( argc != 2 ) {
           fprintf(stderr, "Run test with test number as argument!\n");
           exit(1);
        }

	int testNum = atoi(argv[1]);

	switch(testNum) {
		case 0:
			startTestTask(10);
			sleep(15);
			break;
		case 1:
			qtest_block_receive();
			break;
		case 2:
			qtest_nonblock_receive();
			break;
                case 3:
                        qtest_nonblock_send();
                        break;
                case 4:
                        qtest_block_send();
                        break;
		case 5:
			qtest_performance();
			break;
                case 6:
                        qtest_concurrent();
                        break;
		case 7:
			intervalTimerTest();
			break;
		case 8:
			fileSystemTest();
			break;
		case 9:
			validateFileTest();
			break;
		default:
			printf("Huh? %d\n", testNum);
			break;
	}
	
    return 0;
}

