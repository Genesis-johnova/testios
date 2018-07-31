/* ecdh-test.c

   Copyright (C) 2014 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

#include "testutils.h"

static void
set_point (struct ecc_point *p,
	   const char *x, const char *y)
{
  mpz_t X, Y;
  mpz_init_set_str (X, x, 0);
  mpz_init_set_str (Y, y, 0);
  if (!ecc_point_set (p, X, Y))
    die ("Test point not on curve!\n");

  mpz_clear (X);
  mpz_clear (Y);
}
  
static void
set_scalar (struct ecc_scalar *s,
	    const char *x)
{
  mpz_t X;
  mpz_init_set_str (X, x, 0);
  ecc_scalar_set (s, X);
  mpz_clear (X);
}

static void
check_point (const char *name, const char *label,
	     const struct ecc_point *P,
	     const struct ecc_point *R)
{
  mpz_t px, py, rx, ry;

  mpz_init (px);
  mpz_init (py);
  mpz_init (rx);
  mpz_init (ry);

  ecc_point_get (P, px, py);
  ecc_point_get (R, rx, ry);

  /* FIXME: Should have a public point compare function */
  if (mpz_cmp (px, rx) != 0 || mpz_cmp (py, ry) != 0)
    {
      fprintf (stderr, "Failed %s %s\np_x = ", name, label);
      mpz_out_str (stderr, 10, px);
      fprintf (stderr, "\nr_x = ");
      mpz_out_str (stderr, 10, rx);
      fprintf (stderr, " (expected)\np_y = ");
      mpz_out_str (stderr, 10, py);
      fprintf (stderr, "\nr_y = ");
      mpz_out_str (stderr, 10, ry);
      fprintf (stderr, " (expected)\n");
      abort ();      
    }
  mpz_clear (px);
  mpz_clear (py);
  mpz_clear (rx);
  mpz_clear (ry);
}

static void
test_dh (const char *name, const struct ecc_curve *ecc,
	 const char *a_priv, const char *ax, const char *ay,
	 const char *b_priv, const char *bx, const char *by,
	 const char *sx, const char *sy)
{
  struct ecc_point A, B, S, T;
  struct ecc_scalar A_priv, B_priv;

  ecc_scalar_init (&A_priv, ecc);
  set_scalar (&A_priv, a_priv);
  ecc_point_init (&A, ecc);
  set_point (&A, ax, ay);

  ecc_scalar_init (&B_priv, ecc);
  set_scalar (&B_priv, b_priv);
  ecc_point_init (&B, ecc);
  set_point (&B, bx, by);

  ecc_point_init (&S, ecc);
  set_point (&S, sx, sy);

  ecc_point_init (&T, ecc);

  ecc_point_mul_g (&T, &A_priv);
  check_point (name, "a g", &T, &A);

  ecc_point_mul (&T, &B_priv, &T);
  check_point (name, "b (a g)", &T, &S);

  ecc_point_mul_g (&T, &B_priv);
  check_point (name, "b g", &T, &B);

  ecc_point_mul (&T, &A_priv,  &T);
  check_point (name, "a (b g)", &T, &S);

  ecc_scalar_clear (&A_priv);
  ecc_scalar_clear (&B_priv);

  ecc_point_clear (&A);
  ecc_point_clear (&B);
  ecc_point_clear (&S);
  ecc_point_clear (&T);  
}

void
test_main(void)
{
  test_dh ("secp-192r1", &nettle_secp_192r1,
	   "3406157206141798348095184987208239421004566462391397236532",
	   "1050363442265225480786760666329560655512990381040021438562",
	   "5298249600854377235107392014200406283816103564916230704184",
	   "738368960171459956677260317271477822683777845013274506165",
	   "2585840779771604687467445319428618542927556223024046979917",
	   "293088185788565313717816218507714888251468410990708684573",
	   "149293809021051532782730990145509724807636529827149481690",
	   "2891131861147398318714693938158856874319184314120776776192");

  test_dh ("secp-224r1", &nettle_secp_224r1,
	   "1321072106881784386340709783538698930880431939595776773514895067682",
	   "6768311794185371282972144247871764855860666277647541840973645586477",
	   "2880077809069104378181313860274147139049600284805670362929579614547",
	   "13934723037778859565852601874354272638301919827851286722006496784914",
	   "373124771833407982305885866158843810218322878380632071540538232035",
	   "24223309755162432227459925493224336241652868856405241018762887667883",
	   "8330362698029245839097779050425944245826040430538860338085968752913",
	   "24167244512472228715617822000878192535267113543393576038737592837010");	   

  test_dh ("secp-256r1", &nettle_secp_256r1,
	   "94731533361265297353914491124013058635674217345912524033267198103710636378786",
	   "22441589863306126152768848344973918725077248391248404659242620344938484650846",
	   "8673475622926171928656873398933611700804732317466515884933832073457396747355",
	   "97657865959185011849283028361556797595752581630732610898393589042714626616209",
	   "18453500628354973083413728373777272885280811435138222441593126858566687017580",
	   "14365748655141740924607822284126054269177292284541187981786689038777833170313",
	   "102958799567030688009123101477538973715497039396202015119148334812951370853564",
	   "29188877854984806245046208182450375893010623119030341548941791125497546766367");

  test_dh ("secp-384r1", &nettle_secp_384r1,
	   "39086550219018474560700767788227987514008150214902287969462741484831311917159729009715909108606822193356890811565070",
	   "15536343869384820642787280162462493474000839389760580357050317691132784247078954166759523572989472049798969369413707",
	   "23268351460749985365652822073294615614961429585671989812206213135127969284347174876010177880230302801199500921999966",
	   "36869963309577906178833120963925446333578086292605692048464445726274368063284094788012795873582576522541658781990645",
	   "6571571183519639697971973492227725184968062063941037806786906539419849188357322949908539215960508669158121817812397",
	   "36555212611228586427448926841660565534959679681904941933188284044726925984417589749068550977832780023128545833460008",
	   "27780263733159299625371532605243698753833039933618994121416145881861678645978369807598146716869504289033472077532789",
	   "12327518461490664021199432424728005314646140038116972426756705356672414772151215711157356913456651047992140493843405");

  test_dh ("secp-521r1", &nettle_secp_521r1,
	   "1177787298234877762125077260641419691552146813662613924864132680693789861345339466386194840381422980702458955378518702648732728796955434922249345867267377826",
	   "3168153642368000846168628288850857848098131369578410603904155841373678828215434925507474033105518841999665785152501356092020415699294327720257651796364374116",
	   "278603899104240796379373331240296114411332466119196525390128418935585486485808560319073463912513286987331907013829243645911963547435764718505394265715321106",
	   "4632844957395758597246278843156350179301194123641664447791935593091018103746003967476919616681982477804041933745387575872964923485212972039478646226080044590",
	   "3278857364905061449863537070675297207767865967146919975942590789168732752489407699106980407552332044280575891715425195464227794423128203118286002006478070253",
	   "4488572162727491199625798812850846214916160870437505769058530973184916706326908828109446998319674522651965593412129100088877891410841200092694907512496020182",
	   "2126311732129869456512627735193938710331935978955001830871465201548004444073866677974896970734635601049909886616595755762740651165670628002084824920216966370",
	   "4803556648772727869384704240411011976585308117802975396033423138930126997561438092192867119930177133880625991019440171972612468402200399449807843995563872782");

  /* NOTE: This isn't the standard way to do curve25519
     diffie-hellman, but it tests that the ecc_point interface works
     also with curve25519. */
  test_dh ("curve25519", &_nettle_curve25519,
	   "238301186166219052901200372289459967515481170332211409964804596991365959539",
	   "14283836751943535877833976277675258994717521964638468784408792140505262281235",
	   "43912344711849354965202408139054167824861850336739416536288592824181793690574",
	   "3795950278952272509684177709511717492358770264218705926196469999516028451559",
	   "9468726108732441384988851273894214794301501512287024874346147472389705411936",
	   "38072138078045635808869930165213470653418146012939584392304609812494425185763",
	   "10481077163111981870382976851703705086808805457403127024129174358161599078055",
	   "29260211489972704256554624312266763530759418996739976957020673870747051409679");
}
