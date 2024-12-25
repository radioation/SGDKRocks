
fix16 ApproxAtan(fix16 z)
{
    const fix16 n1 = FIX16(0.97239411f);
    const fix16 n2 = FIX16(-0.19194795f);
    return fix16Mul( n1  +fix16Mul( n2 ,fix16Mul( z, z)), z);
}


u16 ApproxAtan2(fix16 y, fix16 x)
{
    if (x != FIX16(0.0))
    {
				s16 ax = fix16ToInt(x);
				s16 ay = fix16ToInt(y);
        if (abs(x) > abs(y))
        {
		        //KLog_F2x( 4, " abs x > abs y   y : ", y, " x: ", x );
            const fix16 z = fix16Div( y, x );
					  const fix16 tempAtan = ApproxAtan(z);
            if (x > FIX16(0.0))
						{
							//return ApproxAtan(z);
						 if (y >= FIX16(0.0)) {
								// all positive: Lower half of QUAD I  ( 0-PI/4  -> 0-127 )
								//   I = m*atan + b  | b = 0,  m = 127/(0.82)  = 154.878  << remember, we're approximating
								fix16 i = fix16Mul(FIX16(154.878),tempAtan );
								return fix16ToInt(i);
							} else {
								// x positive, y negative: upper half of QUAD IV ( -pi/4 to 0 -> 897 through 1024 )
								//   I = m*atan + b  | b = 1024,  m = 127/(-0.82)  =  161.701
								fix16 i =  fix16Mul(FIX16(154.878),tempAtan) + FIX16(1024);
								return fix16ToInt(i);
							}
						}
						else if (y >= FIX16(0.0))
						{
							//KLog_F2x(4, "    y > 0  y : ", y, " x: ", x);
							// atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
							// x negative, y positive   Lower half of QUAD II
							//  ( 3PI/4-PI  -> 384-512 )
							//   m = 127/0.82 = 154.878  b = 24.683
							//return fix16Add(ApproxAtan(z), PI_1);
								fix16 i =  fix16Mul(FIX16(154.878),tempAtan + PI_1)+ FIX16(24.683);
								return fix16ToInt(i);
						}
						else
						{
							//KLog_F2x(4, "     ---  y : ", y, " x: ", x);
							// atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
							// x negative, y negative   Upper half of QUAD III
							//return fix16Sub(ApproxAtan(z), PI_1);
							//   
							//   m = 127/0.82 = 154.878  b = 998.317
							fix16 i = fix16Mul(FIX16(154.878),(tempAtan-PI_1))+ FIX16(998.317);
							return fix16ToInt(i);

						}
				}
				else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
				{
						//KLog_F2x(4, " abs x < abs y   y : ", y, " x: ", x);
						const fix16 z = fix16Div(x, y);
					  const fix16 tempAtan = ApproxAtan(z);
						if (y > FIX16(0.0))
						{
							// atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
							//return fix16Sub(PI_2, ApproxAtan(z));
							//   m = 255/1.50 = 170      b = -11.4
							fix16 i = fix16Mul(FIX16(170.0),(PI_2-tempAtan))- FIX16(11.4);
							return fix16ToInt(i);
						}
						else
						{
							// atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
							//return fix16Sub(-PI_2, ApproxAtan(z));
							//   m = 255/1.50 = 170      b = 1034.4
							fix16 i = ( fix16Mul(FIX16(-170.0),(PI_2+tempAtan))+ FIX16(1034.4));
							return fix16ToInt(i);
						}
				}
		}
		else
		{
				if (y > FIX16(0.0)) // x = 0, y > 0
				{
						return 256; // PI_2
				}
				else if (y < FIX16(0.0)) // x = 0, y < 0
				{
						return 768;// -PI_2;
				}
		}
		return FIX16(0.0); // x,y = 0. Could return NaN instead.
}

