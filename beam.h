/* $Id: beam.h,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Beam functions
 * Created: 2002-05-02
 */

#define BEAM_DESC "UniMatrix Database"
#define BEAM_NAME "UniMatrix"
#define BEAM_ENDG ".umx"

#define BEAM_COURSE 3
#define BEAM_SEMESTER 2

extern void BeamCourse(UInt16 courseIndex);
extern void BeamCourseByCID(UInt16 cid);
extern Err BeamReceive(DmOpenRef cats, DmOpenRef dogs, ExgSocketPtr socketPtr);
extern void BeamSemester(UInt16 category);
