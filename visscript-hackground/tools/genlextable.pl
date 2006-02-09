#!/usr/bin/perl

my @lexmap = (
	"/",			# 1: Comment, Multi-line comment, Div punctuator
	"[\$a-zA-Z_]",		# 2: Identifier
	"[0-9]",		# 3: Constant
	"[\\.]",		# 4: Floatingpoint constant
	"[\\,;=\\(\\)&\\-+*%|]",# 5: Punctuators
	"\n",			# 6: Newline
	"\t",			# 7: Tab
#	"[ \\v\\f]",		# 8: Whitespace
	"[ \x0b\x0c]",		# 8: Whitespace
);


for ($i=0; $i < 256; $i++) {
	$pmap[$i] = 0;
}

for ($j=0; $j < @lexmap; $j++) {
	print STDERR "Testing: $j = $lexmap[$j]\n";
	for ($i=0; $i < 256; $i++) {
		$regexp = $lexmap[$j];
		if (chr($i) =~ /$regexp/) {
			$pmap[$i] = 1 + $j;
		}
	}
}

$max = 256;
while ($max > 0 && ($pmap[$max-1] == 0)) { $max--; }

print STDERR "Max: $max\n";

print "static unsigned char lextab[256] = {\n\t";
for ($i=0; $i < $max; $i++) {
	print $pmap[$i], ",";
	if (($i + 1) % 32 == 0) {
		print "\n\t";
	}
}

print "\n};\n";
