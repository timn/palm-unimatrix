<?php

/* $Id: changelog.php,v 1.2 2003/06/18 15:04:44 tim Exp $ */

$title="niemueller.de - palm software - UniMatrix - changelog";
$meta_desc="A software to manage your daily university business.";
$meta_keyw="Palm, PalmOS, university, planer, business, beaming, semester, verwalten, changes, changelog";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

// relative to $DOCUMENT_ROOT
$changesfile = "software/palm/unimatrix/CHANGES";

box_begin("Uni Matrix - ChangeLog", "Last modified ".date ("F d Y H:i:s.", getlastmod()));
echo "<br/>\n";
print_heading_customimage("UniMatrix - ChangeLog", "", "unimatrix_big.png", "index.php");
?>


<div class="maintext"><pre>
<?php

$handle = fopen ("$DOCUMENT_ROOT/$changesfile", "r");
while (!feof ($handle)) {
  $buffer = fgets($handle, 4096);
  echo $buffer;
}
fclose ($handle);

?>
</pre></div>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
