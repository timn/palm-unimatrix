<?php

/* $Id: faq.php,v 1.2 2003/06/18 11:40:41 tim Exp $ */

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

<br/><br/>
<b>Q:</b> Can you add support for lessons that only appear every two weeks?<br/>
<b>A:</b> Users who have course A in say even weeks and B in odd weeks have asked
to add this feature. Right now I have no plans to implement that feature since only
two or three asked for this feature. Maybe after 1.0.

<br/><br/>
<b>Q:</b> Where is this alarm feature? I can't find it!<br/>
<b>A:</b> You are probably using a handheld with PalmOS version &lt; 4. Since I
have decided to use the AttentionManager API to get it done fast those handhelds
are not supported right now. Maybe after 1.0.

<br/><br/>
<b>Q:</b> Is this alarm feature save? Won't it ring during exams under any circumstances?<br/>
<b>A:</b> Of course not :-) I have thought about the code for a while and think that
it does what I promise: Do not ring during exams but before them (and if another
exam is written when the alarm should ring it will ring before this exam and so on!).
But of course I cannot guarantee that. If you want I can send you some pseudo code or
the code that UniMatrix uses to find the right ring time and you can check it and
tell me about it... Also there are some side conditions: Is your clock in sync
with the university time (especially on short notification times), have you entered
ALL exams?


<br/><br/><br/>
<a href="index.php">Back</a>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
