<?php

/* $Id: faq.php,v 1.1 2003/02/06 21:27:23 tim Exp $ */

$title="niemueller.de - palm software - UniMatrix - FAQ";
$meta_desc="A software to manage your daily university business.";
$meta_keyw="Palm, PalmOS, university, planer, business, beaming, semester, verwalten";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

box_begin("Uni Matrix", "Last modified ".date ("F d Y H:i:s.", getlastmod()));
?>
<br/>

<table border="0" cellpadding="4">
 <tr>
  <td rowspan="2" valign="top"><a href="index.php"><img src="unimatrix_big.png" border="0" alt="Uni Matrix"></a></td>
  <td><h3>Uni Matrix</h3>
 </tr>
 <tr>
  <td><h4>Software for planning your everyday business at university</h4></td>
 </tr>
</table>

<br/>
<a href="index.php">Back</a>

<h5>FAQ - Frequently asked questions</h5>
<b>Q:</b> Can you add support for lessons before 08:00 and after 20:00?<br/>
<b>A:</b> The support is already implemented. You will note, that you can set times
all over the day. To see courses not in the range from 08:00 to 20:00 just you the up and
down hard buttons on the bottom of your Palm. That will show you the range from
00:00 to 08:00 and from 20:00 to 00:00.
<br/><br/>
<b>Q:</b> Can you add (your-favorite-feature)?<br/>
<b>A:</b> The question I will ask then: Is there a user base for this feature. I mean
drawing the events like on a radar screen definitely looks cool but it does not really
help anybody. So ask yourself: Would that feature useful at all and will it be useful
for more people and not just yourself? Firstly I do not have the time to add all the features
and secondly I want to keep the code base small... If you still think this feature should go in
please send it :-)

<br/><br/>
<b>Q:</b> Can you compile a version that supports PalmOS version less than 3.5?<br/>
<b>A:</b> No, I can't. The 3.5 feature sets really made my life easier to this software done and I do
not plan to write more glue to make it run on older versions. Sorry.

<br/><br/><br/>
<a href="index.php">Back</a>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
