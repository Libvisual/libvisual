#include "jess.h"

int detect_beat(JessPrivate *priv);
void spectre_moyen(JessPrivate *priv, short data_freq_tmp[2][256]);
void C_dEdt_moyen(JessPrivate *priv);
void C_dEdt(JessPrivate *priv);
void C_E_moyen(JessPrivate *priv, short data_freq_tmp[2][256]);
float energy(JessPrivate *priv, short data_freq_tmp[2][256], int type_E);
int start_ticks(JessPrivate *priv);
float time_last(JessPrivate *priv, int i, int reinit);
inline void ips(JessPrivate *priv);
