<?php

/* $Id: changelog.php,v 1.3 2003/10/31 23:36:41 tim Exp $ */

$title="niemueller.de - palm software - UniMatrix - changelog";
$meta_desc="A software to manage your daily university business.";
$meta_keyw="Palm, PalmOS, university, planer, business, beaming, semester, verwalten, changes, changelog";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

// relative to $DOCUMENT_ROOT
$changesfile = "software/palm/unimatrix/CHANGES";

heading_lastmod();
heading("Uni Matrix", "Change Log",
        "/software/palm/unimatrix/unimatrix_big.png", "index.php");
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
