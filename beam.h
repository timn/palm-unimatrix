/* $Id: beam.h,v 1.2 2003/03/13 14:56:47 tim Exp $
 *
 * Beam functions
 * Created: 2002-05-02
 */

#define BEAM_DESC "UniMatrix Database"
#define BEAM_NAME "UniMatrix"
#define BEAM_ENDG ".umx"

#define BEAM_COURSE 3
#define BEAM_SEMESTER 2

extern void BeamCourse(UInt16 courseIndex) SECOND_SECTION;
extern void BeamCourseByCID(UInt16 cid) SECOND_SECTION;
extern Err BeamReceive(DmOpenRef cats, DmOpenRef dogs, ExgSocketPtr socketPtr) SECOND_SECTION;
extern void BeamSemester(UInt16 category) SECOND_SECTION;
