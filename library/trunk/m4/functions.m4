set no prompt
; m4 macro definitions for some elementary math functions and constants in Mathomatic.
m4_define(`sqrt', `(($1)^.5)'); function sqrt(x) = square root of x
m4_define(`cbrt', `(($1)^(1/3))'); function cbrt(x) = cube root of x
m4_define(`exp', `(e^($1))'); function exp(x) = e^x
m4_define(`pow', `(($1)^($2))'); function pow(x, y) = x^y
m4_define(`abs', `(|($1)|)'); function abs(x) = absolute value = |x|
m4_define(`phi', `((1+5^.5)/2)'); phi = the golden ratio constant

set modulus_mode=2 ; mode 1 or 2 required for floor() and ceil().
m4_define(`floor', `(($1)-($1)%1)'); floor(x) = floor function, real x -> integer result
m4_define(`ceil', `(($1)+(-($1))%1)'); ceil(x) = ceiling function, real x -> integer result
m4_define(`int', `(($1)//1)'); int(x) = truncate to integer, real x -> integer result
m4_define(`round', `((($1)+|($1)|/($1)/2)//1)'); round(x) = round to nearest integer; round(0) fails

; Standard trigonometry functions as complex exponentials, argument x is in radians:
m4_define(`sin', `((e^(i*($1))-e^(-i*($1)))/(2i))'); sin(x) = sine of x
m4_define(`cos', `((e^(i*($1))+e^(-i*($1)))/2)'); cos(x) = cosine of x
m4_define(`tan', `((e^(i*($1))-e^(-i*($1)))/(i*(e^(i*($1))+e^(-i*($1)))))'); tan(x) = tangent of x
m4_define(`cot', `(i*(e^(i*($1))+e^(-i*($1)))/(e^(i*($1))-e^(-i*($1))))'); cot(x) = cotangent of x
m4_define(`sec', `(2/(e^(i*($1))+e^(-i*($1))))'); sec(x) = secant of x
m4_define(`csc', `(2i/(e^(i*($1))-e^(-i*($1))))'); csc(x) = cosecant of x

; The following are hyperbolic trigonometry functions:
; sinh(x), cosh(x), tanh(x), coth(x), sech(x), and csch(x).
; These are related to the above trigonometry functions without the "h" appended to the function name.
m4_define(`sinh', `((e^($1)-e^-($1))/2)')
m4_define(`cosh', `((e^($1)+e^-($1))/2)')
m4_define(`tanh', `((e^($1)-e^-($1))/(e^($1)+e^-($1)))')
m4_define(`coth', `((e^($1)+e^-($1))/(e^($1)-e^-($1)))')
m4_define(`sech', `(2/(e^($1)+e^-($1)))')
m4_define(`csch', `(2/(e^($1)-e^-($1)))')

echo Functions are now available.
set prompt
