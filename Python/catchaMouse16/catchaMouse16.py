import catchaMouse16_C

def catchaMouse16_all(data):

	features = [
	'SY_DriftingMean50_min',
	'DN_RemovePoints_absclose_05_ac2rat',
	'AC_nl_036',
	'AC_nl_112',
	'ST_LocalExtrema_n100_diffmaxabsmin',
	'IN_AutoMutualInfoStats_diff_20_gaussian_ami8',
	'CO_HistogramAMI_even_2_3',
	'CO_TranslateShape_circle_35_pts_statav4_m',
	'CO_AddNoise_1_even_10_ami_at_10',
	'PH_Walker_momentum_5_w_momentumzcross',
	'SC_FluctAnal_2_dfa_50_2_logi_r2_se2',
	'PH_Walker_biasprop_05_01_sw_meanabsdiff',
	'CO_HistogramAMI_even_10_3',
	'AC_nl_035',
	'FC_LoopLocalSimple_mean_stderr_chn',
	'CO_TranslateShape_circle_35_pts_std'
	]	

	# The feature functions accept any sequence of numbers (list, tuple,
	# NumPy array, ...). Read a one-shot iterator (e.g., a generator) into a
	# list, since each of the 16 features reads the data.
	try:
		if iter(data) is data:
			data = list(data)
	except TypeError:
		pass  # not iterable: the feature functions raise a TypeError

	featureOut = []
	for f in features:
		featureFun = getattr(catchaMouse16_C, f)
		featureOut.append(featureFun(data))

	return {'names': features, 'values': featureOut}
