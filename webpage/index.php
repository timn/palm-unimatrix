<?php

/* $Id: index.php,v 1.14 2003/11/20 23:10:45 tim Exp $ */

$title="niemueller.de - palm software - UniMatrix";
$meta_desc="A software to manage your daily university business.";
$meta_keyw="Palm, PalmOS, university, planer, business, beaming, semester, verwalten";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

$version="1.0rc1";
$basename="UniMatrix";
$languages=array( "de" => "German/Deutsch",
                  "en" => "English",
                  "pt" => "Portugues",
                  "fr" => "French/Francais",
                  "gr" => "Greek",
                  "es" => "Spanish",
                  "si_latin1" => "Slovenian (Latin1)",
                  "si_latin2" => "Slovenian (Latin2)",
                  "si_cp1250" => "Slovenian (cp1250)",
                  "nl" => "Dutch",
                  "pl_iso8859-1" => "Polish (ISO 8859-1)",
                  "pl_iso8859-2" => "Polish (ISO 8859-2)",
                  "cz_iso8859-2" => "Czech (ISO 8859-2)",
                  "cz_cp1250" => "Czech (cp1250)",
                  "it" => "Italian",
                  "ca" => "Catalan",
                  "kr" => "Korean",
                  "il" => "Hebrew (Israel)",
                );

$preversion=0;

heading_lastmod();
heading("Uni Matrix",
        "Software for planning your everyday business at university",
        "/software/palm/unimatrix/unimatrix_big.png");
?>

<div class="maintext">
<h5>Introduction</h5>
This PalmOS applications allows you to manage your lessons on your Palm.
If you open the program it jumps to the upcoming lessons so that you see fast
what comes next. It shows all needed information on one screen.<br/>
Some features:
<ul>
 <li>Multiple semesters: Up to 16 semester can be managed.</li>
 <li>Manage courses, events and exams</li>
 <li>Beaming Support: Courses can be beamed to another palm (You can also beam whole semesters).</li>
 <li>Keep your data easily in one place. Homepage, email, phone, everything about a lecture in your Palm.</li>
 <li>Alarm for exams (but not during an exam, UniMatrix will take care of it, only PalmOS >= 4)</li>
 <li>Notes for events and exams</li>
</ul>

<br/>
Just to be sure that you heard: You will need a PalmOS device to run this
application. You can also use the Palm OS Emulator
<a href="http://www.palmos.com/dev/tools/emulator/" rel="external">POSE</a>.
Linux binaries can be found
<a href="http://sourceforge.net/project/showfiles.php?group_id=2189" rel="external">here</a>.
<br/>
<i>Do not ask for ROM files</i>. I cannot and I do not want to share
my ROMs with you. Go to the mentioned POSE page, there is a description
on how to obtain ROM files from Palm Source directly and legally
without owning a Palm handheld.
<br/><br/>
UniMatrix is available in multiple languages. Supported languages are right now (randomly ordered):
<ul>
 <li>German (Deutsch)</li>
 <li>English</li>
 <li>Portugues (Thanx to Leonardo Dias Vaz de Melo from Brasil)</li>
 <li>French (Thanx to Francis H. and Antoine from France)</li>
 <li>Greek (Thanx to Tom Diamond from Greece)</li>
 <li>Slovenian (Thanx to Bostjan Müller from Slovenia)</li>
 <li>Spanish (Thanx to Antonio Guijarro and Andreu Bassols from Spain)</li>
 <li>Czech (Thanx to Tomas Kolousek)</li>
 <li>Dutch (Thanx to Colin Helvensteijn)</li>
 <li>Polish (Thanx to Tomasz Zielinski)</li>
 <li>Catalan (Thanx to Andreu Bassols from Spain)</li>
 <li>Korean (Thanx to Unknown from Korea (do not know which part))</li>
 <li>Hebrew (Thanx to Gil Zlotnik)</li>
</ul>
If you want to help and translate UniMatrix to another language please
<a href="mailto:tim@niemueller.de?Subject=Translate UniMatrix">contact</a> me and I will
send you the language file.


<?include("$DOCUMENT_ROOT/software/palm/sponsoring.inc.php");?>


<h5>FAQ - Frequently Asked Questions</h5>
There are some questions people are asking again and again. Read the common answers
<a href="faq.php">here</a>. <span style="color: red;">Please read this FAQ <span style="text-decoration: underline;">before</span> sending
<span style="text-decoration: underline;">any</span> questions!</span>


<h5>News</h5>
<b>2003/11/21</b><br/>
Release of UniMatrix 1.0rc1. This version fixes several <i>critical</i> bugs that could
cause data loss and confusion. Also the "crash after reset" bug has been fixed. Several
minor code improvements. More "intelligent" dialog for adding events. It tries to
"guess" the color now if there is already an event for the selected course. You should
upgrade immediately! If this version does not throw any more bugs I will make this 1.0
and will work on some new features for 1.1 :-)<br/>
<b>2003/06/18</b><br/>
Release of UniMatrix 0.9. Some major new features. This is the "feature freeze" release
for UniMatrix 1.0 so please test it and gremling around to get it stable for
1.0. New features include alarm for exams (only PalmOS >= 4) and notes for events
and exams and new caching to get drawing faster (also some flicker removement still
needs to be done), ability to show first letters of course name on main page in
event boxes.<br/>
<b>2002/11/28</b><br/>
Release of UniMatrix 0.8. Getting pretty close to the 1.0 release. Lots of new features and several bug
fixes. Integrated with other new software like <a href="../unimensa/">UniMensa</a> and <a href="../unichat/">UniChat</a>.
Download and try it out.<br/>
<b>2002/10/16</b><br/>
Release of UniMatrix 0.7. Several users have tested this version and it seems to work pretty well. But
just to be sure make a backup before installing this version (as you should do with every new software...).
See below what changes have been made and which features I added.
If you find any bugs please send me a <a href="mailto:unimatrix-bug@niemueller.de">mail</a> describing the
problem (please note OS and hardware you use).

<h5>Requested and/or planned features</h5>
<ul>
 <li>Be able to enter events that occur only every second week</li>
 <li>"Zoom" - Have only 6 hours on one screen instead of 12 for example</li>
 <li>Enter grades (probably not)</li>
 <li>Enter feature that shows time to end of current lessens and to start of next lesson</li>
 <li>Show remaining time for current event, this seems to be wanted by many people
that are bored in their lessons :-)</li>
</ul>


<h5>Thanks</h5>
Thanks for all the user contributions for those ideas! Especially to Leonardo Dias Vaz de Melo from Brasil
and to Andy Y. Lin from the MIT.
<br/><br/>
A special thanks to my friend Tobias "The Bugfinder" Weyand who greatly helped to reduce my Bug ratio in this
software (and others...). All remaining bugs are my fault. I probably inserted them after Tob's checks...


<h5>Screenshots</h5>
Some screenshots will give you an impression of the software.
<br/><br/>
<table border="0" width="100%" cellpadding="0">
 <tr>
  <td align="center" style="width:33%;"><img src="screens/main.png" style="border: 1px solid #c0c0c0;" alt="Main screen" /></td>
  <td align="center" style="width:34%;"><img src="screens/edit_course.png" style="border: 1px solid #c0c0c0;" alt="Edit course" /></td>
  <td align="center" style="width:33%;"><img src="screens/courselist.png" style="border: 1px solid #c0c0c0;" alt="Course List" /></td>
 </tr>
 <tr>
  <th>Main screen. UniMatrix can handle up to 16 semesters</th>
  <th>Edit course</th>
  <th>Coure List</th>
 </tr>

 <tr>
  <td><br/>&nbsp;</td>
 </tr>

 <tr>
  <td align="center"><img src="screens/main2.png" style="border: 1px solid #c0c0c0;" alt="Main screen with short types" /></td>
  <td align="center"><img src="screens/edit_event.png" style="border: 1px solid #c0c0c0;" alt="Edit Event" /></td>
  <td align="center"><img src="screens/exams.png" style="border: 1px solid #c0c0c0;" alt="Exams" /></td>
 </tr>
 <tr>
  <th>UniMatrix can show the types for courses</th>
  <th>Edit event</th>
  <th>Exams</th>
 </tr>
</table>

<h5>What users said about UniMatrix</h5>
<ul>
<li>Dear Mr. Niemueller. Thank you for Unimatrix 0.4!
It is EXACTLY what I have been looking for. I am not a student, but a teacher.
I have three part-time teaching jobs, and UniMatrix will help me avoid schedule conflicts.
<i>Bill Hogue (Kobe, Japan)</i>
[Found on <a href="http://www.freewarepalm.com/educational/unimatrix.shtml" rel="external">freewarepalm.com</a>]
</li>
<li>Very good program! well coded, easy to use and very handy. I recommend it to anyone at school or uni!
<i>Steg</i>
[Found on <a href="http://www.freewarepalm.com/educational/unimatrix.shtml" rel="external">freewarepalm.com</a>]
</li>
<li>The developer seems to know exactly what a student needs and has implemented it in his program.
This is what I would probably classify as one of those really functional program. I really wish I had
this when I was in university. Guess I'll give this program a try anyway. It looks good!
<i>Kuan Ming Keong (Singapore)</i>
[Found on <a href="http://www.spug.net/forums/showthread.php?threadid=30075" rel="external">SPUG</a>]</li>
<li>A simple, well-organized app that does its job well. It should be useful to many people who need to keep
track of weekly events or schedules.<i>Bill H.</i>
[Found on <a href="http://www.freewarepalm.com/educational/unimatrix.shtml" rel="external">freewarepalm.com</a>]</li>
<li>[Zip-zilla] transfers everything from Timetable Pro to UniMatrix
<i>Andre Tan, Singapore</i>
[Found on <a href="http://www.freewarepalm.com/educational/unimatrix.shtml" rel="external">freewarepalm.com</a>]</li>
</ul>

<h5>UniMatrix in the press</h5>
<ul>
 <li>The German magazine <a href="http://www.nouh.de" rel="external">"Notebooks, Organizer und Handy"</a> has a column
about good freeware. The article can be downloaded <a href="unimatrix_nouh_oct2002.pdf">here</a>. Thanks to
NOuH for permission to do so!</li>
</ul>
<br/>
If you want to write an article about UniMatrix please let me know. The development version I have usually has the features
of tommorrow so you might want to test that also. Please send me a printed or digital version or your writing. Thank you!

<h5>Supported devices</h5>
Any PalmOS handheld device running PalmOS 3.5 or higher should work for UniMatrix.
To get the real UniMatrix feeling you need a handheld with a color display. See the
screenshots why... But it also supports 16 grays so you can use it on a grayscale device, too.
<br/><br/>I have tested UniMatrix on:<br/>
Palm m515 (PalmOS 4.1, color, tested on real device, works perfectly)<br/>
Palm IIIc (PalmOS 3.5, color, tested on real device, works perfectly)<br/>
Palm IIIx (PalmOS 4.0, gray, tested on real device, works perfectly)<br/>
Palm IIIx (PalmOS 3.5, gray, tested on emulator, works perfectly)<br/>
Palm III (Palms OS 4.1, b/w, tested on emulator, works, but looks bad)<br/>

<h5>Changes</h5>
Read complete changelog <a href="changelog.php">here</a>.


<h5>Download</h5>
UniMatrix is released and distributed under the terms of a proprietary
license. You may use the software for free and beam it to your friend.
You may not distribute it under your name. You may distribute it on
so called "Shareware or Freeware CD-ROMs". You must then inform the author
<i>before</i> the CDs are published. The author has the right to deny distribution
of the software without further need of explanation.
<br/>
<span style="color: red;">By downloading and/or using the software you agree
to this license!</span>
<br/><br/>
You may download the Palm install file provided as a .zip and as a .tar.gz file.
<br/>Latest <b>stable</b> version is <span style="color:red;"><?=$version?></span>.
<br/><br/>
<table border="0" cellpadding="0" cellspacing="0">
<? foreach ($languages as $key => $value) { ?>
<?   if (file_exists("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.tar.gz")) { ?>
 <tr>
  <td>UniMatrix <?=$version?> (<?=$value?>)</td>
  <td>&nbsp; &nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-<?=$version?>-<?=$key?>.tar.gz">.tar.gz</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.tar.gz")?> Bytes)</td>
  <td>&nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-<?=$version?>-<?=$key?>.zip">.zip</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.zip")?> Bytes)</td>
 </tr>
<?   } ?>
<? } ?>
</table>
<br/>

</div>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
