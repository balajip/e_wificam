#ifndef _GMAC_H_
#define _GMAC_H_

#ifdef CONFIG_SOC_MOZART
  #include "gmac-mozart.h"
#elif defined(CONFIG_SOC_BEETHOVEN)
  #include "gmac-beethoven.h"
#endif

#endif /* !_GMAC_H_ */
