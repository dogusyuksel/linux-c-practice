#include <stdio.h>
#include <string.h>

int main(void)
{
	char buffer[256] = {0};
	FILE *fp = popen("ifconfig lo", "r");

	if (!fp) {
		perror("popen error");
		return 1;
	}

	while(fgets(buffer, sizeof(buffer), fp)) {
		printf("%s", buffer);
		memset(buffer, 0, sizeof(buffer));
	}

	pclose(fp);

	return 0;
}
