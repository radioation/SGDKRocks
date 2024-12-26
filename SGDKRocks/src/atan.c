#include <genesis.h>

#define PI_1 FIX16(3.142)
#define PI_2 FIX16(1.571)



fix32 ApproxAtan(fix32 z)
{
    const fix32 n1 = FIX32(0.97239411f);
    const fix32 n2 = FIX32(-0.19194795f);
    return fix32Mul( n1  +fix32Mul( n2 ,fix32Mul( z, z)), z);
}


u16 ApproxAtan2(fix32 y, fix32 x)
{
    if (x != FIX32(0.0))
    {
				s16 ax = fix32ToInt(x);
				s16 ay = fix32ToInt(y);
        if (abs(x) > abs(y))
        {
		        //KLog_F2x( 4, " abs x > abs y   y : ", y, " x: ", x );
            const fix32 z = fix32Div( y, x );
					  const fix32 tempAtan = ApproxAtan(z);
            if (x > FIX32(0.0))
						{
							//return ApproxAtan(z);
						 if (y >= FIX32(0.0)) {
								// all positive: Lower half of QUAD I  ( 0-PI/4  -> 0-127 )
								//   I = m*atan + b  | b = 0,  m = 127/(0.82)  = 154.878  << remember, we're approximating
								fix32 i = fix32Mul(FIX32(154.878),tempAtan );
								return fix32ToInt(i);
							} else {
								// x positive, y negative: upper half of QUAD IV ( -pi/4 to 0 -> 897 through 1024 )
								//   I = m*atan + b  | b = 1024,  m = 127/(-0.82)  =  161.701
								fix32 i =  fix32Mul(FIX32(154.878),tempAtan) + FIX32(1024);
								return fix32ToInt(i);
							}
						}
						else if (y >= FIX32(0.0))
						{
							//KLog_F2x(4, "    y > 0  y : ", y, " x: ", x);
							// atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
							// x negative, y positive   Lower half of QUAD II
							//  ( 3PI/4-PI  -> 384-512 )
							//   m = 127/0.82 = 154.878  b = 24.683
							//return fix32Add(ApproxAtan(z), PI_1);
								fix32 i =  fix32Mul(FIX32(154.878),tempAtan + PI_1)+ FIX32(24.683);
								return fix32ToInt(i);
						}
						else
						{
							//KLog_F2x(4, "     ---  y : ", y, " x: ", x);
							// atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
							// x negative, y negative   Upper half of QUAD III
							//return fix32Sub(ApproxAtan(z), PI_1);
							//   
							//   m = 127/0.82 = 154.878  b = 998.317
							fix32 i = fix32Mul(FIX32(154.878),(tempAtan-PI_1))+ FIX32(998.317);
							return fix32ToInt(i);

						}
				}
				else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
				{
						//KLog_F2x(4, " abs x < abs y   y : ", y, " x: ", x);
						const fix32 z = fix32Div(x, y);
					  const fix32 tempAtan = ApproxAtan(z);
						if (y > FIX32(0.0))
						{
							// atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
							//return fix32Sub(PI_2, ApproxAtan(z));
							//   m = 255/1.50 = 170      b = -11.4
							fix32 i = fix32Mul(FIX32(170.0),(PI_2-tempAtan))- FIX32(11.4);
							return fix32ToInt(i);
						}
						else
						{
							// atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
							//return fix32Sub(-PI_2, ApproxAtan(z));
							//   m = 255/1.50 = 170      b = 1034.4
							fix32 i = ( fix32Mul(FIX32(-170.0),(PI_2+tempAtan))+ FIX32(1034.4));
							return fix32ToInt(i);
						}
				}
		}
		else
		{
				if (y > FIX32(0.0)) // x = 0, y > 0
				{
						return 256; // PI_2
				}
				else if (y < FIX32(0.0)) // x = 0, y < 0
				{
						return 768;// -PI_2;
				}
		}
		return FIX32(0.0); // x,y = 0. Could return NaN instead.
}

