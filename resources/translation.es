/* $Id: translation.es,v 1.1 2003/02/06 21:27:23 tim Exp $ */

translation "spanish"
begin

/* about translator only name is displayed */
  "translator" = "Antonio Guijarro"
  "trans_mail" = "macmanAG@netscape.net"
  "trans_hdev" = "Visor Pro"
  "trans_palm" = "Unknown"
  "trans_trby" = "Spanish translation by:"


/* common */
  "com_cur" = "Actual"
  "com_man" = "Manejar Semestres"


/* menu */
  "men_addc" = "A?adir curso"
  "men_adde" = "A?adir evento"
  "men_cl" = "Lista de cursos"
  "men_ab" = "Informaci?n"
  "men_opt" = "Opciones"
  "men_sets" = "Ajustes"
  "men_ex" = "Exams"
  "men_beam" = "Beam Sem."


/* alerts */
  "al_ok" = "OK"
  "al_rominc1" = "Sistema incompatible"
  "al_rominc2" = " Se requiere la Version 3.5 del sistema o superior para utilizar esta aplicaci?n."
  "al_noc1" = "Error: No hay curso"
  "al_noc2" = "Se debe crear un curso antes de a?adir un evento."
  "al_noc3" = "OK"
  "al_nr1" = "Error: No hay clase"
  "al_nr2" = "Debes a?adir una clase"
  "al_nomem1" = "Memoria insuficiente"
  "al_nomem2" = "No se pudo asignar memoria."
  "al_del1" = "ÀEstas seguro de borrar?"
  "al_del2" = "ÀBorrar el elemento seleccionado(^1 [^2], ^3)?"
  "al_dely" = "Si"
  "al_deln" = "No"
  "al_ebb1" = "Termina antes de empezar"
  "al_ebb2" = "El final del evento es anterior al comienzo. Esto es imposible."
  "al_invend1" = "Hora no v?lida"
  "al_invend2" = "ÁLa hora debe encontrarse entre las 8:00 y las 20:00!"
  "al_delc1" = "ÀEstas seguro de borrar?"
  "al_delc2" = "Estas seguro de borrar este curso(^1) y todos los eventos dependientes del mismo?^23"
  "al_delcy" = "Si"
  "al_delcn" = "No"
  "al_ni1" = "No hay nada seleccionado."
  "al_ni2" = "Debes seleccionar algo que editar/borrar."
  "al_ecn1" = "Sin nombre"
  "al_ecn2" = "ÁDebes asignarle un nombre al curso!"
  "al_ect1" = "Sin profesor"
  "al_ect2" = "Debes incluir el nombre del profesor(a)!"
  "al_unt1" = "Sin hora"
  "al_unt2" = "Los eventos tienen que ser a una determinada hora."
  "al_old1" = "Base de datos antigua"
  "al_old2" = "Esta base de datos ha sido creada con una versi?n antigua de Unimatrix. No puede ser convertida. ÀPuedo convertir la base de datos ahora?"
  "al_old3" = "Si"
  "al_old4" = "No"
  "al_ctd1" = "Valores inv?lidos"
  "al_ctd2" = "ÁDebes dar el nombre normal y el nombre corto!"
  "al_ct_del1" = "ÀEsta seguro de borrar?"
  "al_ct_del2" = "Borrar tipo de curso ^1 [^2]?^3"
  "al_ex_del1" = "Really delete?"
  "al_ex_del2" = "Really delete exam for ^1 on ^2 at ^3?"
  "al_timecol1" = "Time Collision"
  "al_timecol2" = "The given times collide with an already entered event."
  "al_beamdbfail1" = "Database Error"
  "al_beamdbfail2" = "Could not open database. Start UniMatrix once before receiving a beam to initialise database."
  "al_beamnocat1" = "Insufficient Memory"
  "al_beamnocat2" = "There are already the maximum number of categories possible. Delete an old category and try again."


/* about */
  "ab_title" = "Sobre UniMatrix"
  "ab_ok" = "OK"
  "ab_d1" = "Software para organizar"
  "ab_d2" = " tu trabajo diario en la universidad"
  "ab_get" = "Consigue una versi?n actualizada en"


/* beam */
  "b_title" = "Elegir Semestre"
  "b_d1" = "Selecciona el semestre en"
  "b_d2" = "el cual el curso recibido"
  "b_d3" = "sera guardado."
  "b_sem" = "Semestre:"
  "bs_d1" = "This will create a new semester"
  "bs_d2" = "that the received entries will"
  "bs_d3" = "be saved to."


/* course list */
  "cl_title" = "Lista de cursos"
  "cl_desc" = "Todos los cursos en este semestre:"


/* course details (add and edit) */
  "cd_title" = "Editar Curso"
  "cd_name" = "Nombre"
  "cd_type" = "Tipo"
  "cd_teacher" = "Profesor(a)"
  "cd_email" = "Email"
  "cd_website" = "P?gina Web"
  "cd_phone" = "Tel?fono"


/* event details (add and edit) */
  "ed_title" = "Editar Evento"
  "ed_course" = "Curso"
  "ed_day" = "Dia"
  "ed_time" = "Hora"
  "ed_title" = "Seleccionar Hora"
  "ed_color" = "Color"
  "ed_room" = "Clase"
  "ed_mo" = "Lunes"
  "ed_tu" = "Martes"
  "ed_we" = "Miercoles"
  "ed_th" = "Jueves"
  "ed_fr" = "Viernes"
  "ed_sa" = "S?bado"
  "ed_r" = "R"
  "ed_g" = "G"
  "ed_b" = "B"


/* settings */
  "sets_title" = "Ajustes"
  "sets_day" = "Sabado es dia de trabajo"
  "sets_showtype" = "Show short types for events"
  "sets_showtime" = "Show timeline for current time"


/* gadget */
  "gad_mo" = "Lun"
  "gad_tu" = "Mar"
  "gad_we" = "Mier"
  "gad_th" = "Jue"
  "gad_fr" = "Vie"
  "gad_sa" = "Sab"


/* Course types */
  "ct_title" = "Tipos de Cursos"
  "ct_desc" = "Elige tipo de curso a editar:"
  "ct_edct" = "Editar Tipos..."
  /* Course type defaults for database initialization */
  "ct_lec" = "Clase"
  "ct_ex" = "Pr?ctica"
  "ct_bex" = "Sesi?n"
  "ct_sem" = "Seminario"
  "ct_psem" = "Tutoria"
  "ct_rep" = "Estudio"
  "ct_unk" = "Desconocido"
  /* The short names may be only 3 chars in length! */
  "ct_sh_lec" = "Cl"
  "ct_sh_ex" = "Pr"
  "ct_sh_bex" = "Ses"
  "ct_sh_sem" = "Sem"
  "ct_sh_psem" = "Tut"
  "ct_sh_rep" = "Est"
  "ct_sh_unk" = "?"


/* Course type details */
  "ctdet_title" = "Course Type Details"
  "ctdet_name" = "Name"
  "ctdet_short" = "Short"


/* Exam list and exam details */
  "ex_title" = "Exams"
  "exd_title" = "Exam Details"
  "exd_course" = "Course"
  "exd_date" = "Date"
  "exd_time" = "Time"
  "exd_length" = "Length"


end
