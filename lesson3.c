#include <stdio.h>
int main(void)
{
    int price,shares;
    float num,denom,value;
    printf("share price(format: integer num denom)\n");
    scanf("%d%f%f",&price,&num,&denom);
    printf("please input number of shares\n");
    scanf("%d",&shares);
    value = (price + num/denom)*shares;
    printf("value of shares: %f\n",value);
    return 0;
}