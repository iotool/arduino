# ATtiny13A_Serial_CoMCU

## Serial Protocol

```
> "4:0101010100AAAA\n"
> "3:01010101010101\n"
> "2:00000001010101\n"
> "1:01000000000000\n"

buffer[11] = 0

> "C:1\n"
< "J=@@]\n"
> "4:0101010100A8A8\n"
> "3:01010101010101\n"
> "2:00000000010101\n"
> "1:01000000000000\n"
```

### Encode Request

```
String serialCommand(uint8_t pos, uint8_t val) {
  // command to set a value at address with parity
  // encode as ascii code between 64..94
  // because of simple validation algorithm
  // debugging and control charactures ":", "=", "\n"

  char adr = pos;           // address 0..26
       adr += 64;           // move to ascii @..Y
  char vhi = val;
       vhi &= 0b11110000;   // value hex high 16..240
       vhi = vhi >> 4;      // shift to 0..15
       vhi += 64;           // move to ascii @..P
  char vlo = val;
       vlo &= 0b00001111;   // value hex low 0..15
       vlo += 64;           // move to ascii @..P

  char par = adr            // address 
           ^ 61             // "="
           ^ vhi            // value_high
           ^ vlo            // value_low
           ^ 10             // line feed       
           ^ 32;            // move to ascii @..^

  String cmd = "";          // "A=HLP\n"
         cmd += adr;        // @..Y  address
         cmd += "=";        // =     set command
         cmd += vhi;        // @..P  value high
         cmd += vlo;        // @..P  value low
         cmd += par;        // @..^  parity
         cmd += "\n";       // \n    line feed
  return cmd;
}
```

## Serial Input

```
min(parity) = 64
max(parity) = 95

@=@@W  0= 0, 0:87  0
@=@AV  0= 0, 1:86  1
@=@BU  0= 0, 2:85  2
@=@CT  0= 0, 3:84  3
@=@DS  0= 0, 4:83  4
@=@ER  0= 0, 5:82  5
@=@FQ  0= 0, 6:81  6
@=@GP  0= 0, 7:80  7
...
Y=OED 25=15, 5:68  245
Y=OFG 25=15, 6:71  246
Y=OGF 25=15, 7:70  247
Y=OHI 25=15, 8:73  248
Y=OIH 25=15, 9:72  249
Y=OJK 25=15,10:75  250
Y=OKJ 25=15,11:74  251
Y=OLM 25=15,12:77  252
Y=OML 25=15,13:76  253
Y=ONO 25=15,14:79  254
Y=OON 25=15,15:78  255
```


All combinations of partity, address, high and low values

```
<script>
function enc(i) 
{
  return i+64;
}
function xor(N,H,L) 
{
  var P = N ^ 61 ^ H ^ L ^ 10 ^ 32; //  61 ^ 32
  return P;
}
document.writeln('<pre>');
var nx,hx,ix,ixmin=255,ixmax=0;

for (n=0; n<26; n++) 
  for (h=0; h<16; h++) 
    for (l=0; l<16; l++) 
    {
      nx = enc(n);
      hx = enc(h);
      lx = enc(l);
      px = xor(nx,hx,lx);
      if (px<ixmin) ixmin=px;
      if (px>ixmax) ixmax=px;
    }
document.writeln('min(p) = '+ixmin);
document.writeln('max(p) = '+ixmax);
document.writeln();

for (n=0; n<26; n++) 
  for (h=0; h<16; h++) 
    for (l=0; l<16; l++) 
    {
      nx = enc(n);
      hx = enc(h);
      lx = enc(l);
      px = xor(nx,hx,lx);
      document.write(String.fromCharCode(nx)+'=');
      document.write(String.fromCharCode(hx));
      document.write(String.fromCharCode(lx));
      document.write(String.fromCharCode(px)+' ');
      if (n<10) document.write(' '); document.write(n+'=');
      if (h<10) document.write(' '); document.write(h+',');
      if (l<10) document.write(' '); document.write(l+':');
      if (px<10) document.write(' '); document.write(px+'  ');
      document.write(h*16+l);
        document.writeln();
    }

document.writeln('</pre>');
</script>
```
