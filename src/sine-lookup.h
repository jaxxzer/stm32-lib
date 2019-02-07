#include <stdint.h>
uint16_t lookup[] = {
    32767, 32817, 32867, 32918, 32968, 33018, 33068, 33119, 33169, 33219, 33269, 33320, 33370, 33420, 33470, 33521,
    33571, 33621, 33671, 33721, 33772, 33822, 33872, 33922, 33972, 34023, 34073, 34123, 34173, 34223, 34274, 34324,
    34374, 34424, 34474, 34525, 34575, 34625, 34675, 34725, 34775, 34825, 34876, 34926, 34976, 35026, 35076, 35126,
    35176, 35226, 35277, 35327, 35377, 35427, 35477, 35527, 35577, 35627, 35677, 35727, 35777, 35827, 35877, 35927,
    35977, 36027, 36077, 36127, 36177, 36227, 36277, 36327, 36377, 36427, 36477, 36527, 36577, 36627, 36676, 36726,
    36776, 36826, 36876, 36926, 36976, 37025, 37075, 37125, 37175, 37225, 37274, 37324, 37374, 37424, 37473, 37523,
    37573, 37622, 37672, 37722, 37771, 37821, 37871, 37920, 37970, 38019, 38069, 38119, 38168, 38218, 38267, 38317,
    38366, 38416, 38465, 38515, 38564, 38614, 38663, 38712, 38762, 38811, 38861, 38910, 38959, 39009, 39058, 39107,
    39157, 39206, 39255, 39304, 39354, 39403, 39452, 39501, 39550, 39599, 39649, 39698, 39747, 39796, 39845, 39894,
    39943, 39992, 40041, 40090, 40139, 40188, 40237, 40286, 40335, 40383, 40432, 40481, 40530, 40579, 40628, 40676,
    40725, 40774, 40822, 40871, 40920, 40968, 41017, 41066, 41114, 41163, 41211, 41260, 41308, 41357, 41405, 41454,
    41502, 41551, 41599, 41648, 41696, 41744, 41793, 41841, 41889, 41937, 41986, 42034, 42082, 42130, 42178, 42226,
    42274, 42322, 42371, 42419, 42467, 42515, 42562, 42610, 42658, 42706, 42754, 42802, 42850, 42897, 42945, 42993,
    43041, 43088, 43136, 43184, 43231, 43279, 43327, 43374, 43422, 43469, 43517, 43564, 43611, 43659, 43706, 43754,
    43801, 43848, 43895, 43943, 43990, 44037, 44084, 44131, 44178, 44226, 44273, 44320, 44367, 44414, 44461, 44507,
    44554, 44601, 44648, 44695, 44742, 44788, 44835, 44882, 44928, 44975, 45022, 45068, 45115, 45161, 45208, 45254,
    45301, 45347, 45394, 45440, 45486, 45533, 45579, 45625, 45671, 45717, 45763, 45810, 45856, 45902, 45948, 45994,
    46040, 46086, 46131, 46177, 46223, 46269, 46315, 46360, 46406, 46452, 46497, 46543, 46589, 46634, 46680, 46725,
    46771, 46816, 46861, 46907, 46952, 46997, 47042, 47088, 47133, 47178, 47223, 47268, 47313, 47358, 47403, 47448,
    47493, 47538, 47583, 47627, 47672, 47717, 47762, 47806, 47851, 47895, 47940, 47985, 48029, 48073, 48118, 48162,
    48207, 48251, 48295, 48339, 48384, 48428, 48472, 48516, 48560, 48604, 48648, 48692, 48736, 48780, 48823, 48867,
    48911, 48955, 48998, 49042, 49085, 49129, 49172, 49216, 49259, 49303, 49346, 49389, 49433, 49476, 49519, 49562,
    49605, 49649, 49692, 49735, 49778, 49820, 49863, 49906, 49949, 49992, 50034, 50077, 50120, 50162, 50205, 50247,
    50290, 50332, 50375, 50417, 50459, 50502, 50544, 50586, 50628, 50670, 50712, 50754, 50796, 50838, 50880, 50922,
    50964, 51006, 51047, 51089, 51131, 51172, 51214, 51255, 51297, 51338, 51379, 51421, 51462, 51503, 51545, 51586,
    51627, 51668, 51709, 51750, 51791, 51832, 51872, 51913, 51954, 51995, 52035, 52076, 52117, 52157, 52198, 52238,
    52278, 52319, 52359, 52399, 52439, 52480, 52520, 52560, 52600, 52640, 52680, 52720, 52759, 52799, 52839, 52879,
    52918, 52958, 52997, 53037, 53076, 53116, 53155, 53194, 53234, 53273, 53312, 53351, 53390, 53429, 53468, 53507,
    53546, 53585, 53624, 53662, 53701, 53740, 53778, 53817, 53855, 53894, 53932, 53970, 54009, 54047, 54085, 54123,
    54161, 54199, 54237, 54275, 54313, 54351, 54389, 54426, 54464, 54502, 54539, 54577, 54614, 54652, 54689, 54726,
    54764, 54801, 54838, 54875, 54912, 54949, 54986, 55023, 55060, 55097, 55133, 55170, 55207, 55243, 55280, 55316,
    55353, 55389, 55425, 55462, 55498, 55534, 55570, 55606, 55642, 55678, 55714, 55750, 55786, 55821, 55857, 55893,
    55928, 55964, 55999, 56035, 56070, 56105, 56140, 56176, 56211, 56246, 56281, 56316, 56351, 56386, 56420, 56455,
    56490, 56524, 56559, 56594, 56628, 56662, 56697, 56731, 56765, 56799, 56834, 56868, 56902, 56936, 56970, 57003,
    57037, 57071, 57105, 57138, 57172, 57205, 57239, 57272, 57305, 57339, 57372, 57405, 57438, 57471, 57504, 57537,
    57570, 57603, 57635, 57668, 57701, 57733, 57766, 57798, 57831, 57863, 57895, 57927, 57960, 57992, 58024, 58056,
    58088, 58120, 58151, 58183, 58215, 58246, 58278, 58309, 58341, 58372, 58404, 58435, 58466, 58497, 58528, 58559,
    58590, 58621, 58652, 58683, 58713, 58744, 58775, 58805, 58836, 58866, 58896, 58927, 58957, 58987, 59017, 59047,
    59077, 59107, 59137, 59167, 59197, 59226, 59256, 59285, 59315, 59344, 59374, 59403, 59432, 59461, 59490, 59519,
    59548, 59577, 59606, 59635, 59664, 59692, 59721, 59749, 59778, 59806, 59835, 59863, 59891, 59919, 59947, 59975,
    60003, 60031, 60059, 60087, 60115, 60142, 60170, 60197, 60225, 60252, 60280, 60307, 60334, 60361, 60388, 60415,
    60442, 60469, 60496, 60523, 60549, 60576, 60602, 60629, 60655, 60682, 60708, 60734, 60760, 60786, 60812, 60838,
    60864, 60890, 60916, 60941, 60967, 60993, 61018, 61043, 61069, 61094, 61119, 61144, 61170, 61195, 61220, 61244,
    61269, 61294, 61319, 61343, 61368, 61392, 61417, 61441, 61465, 61490, 61514, 61538, 61562, 61586, 61610, 61633,
    61657, 61681, 61704, 61728, 61751, 61775, 61798, 61821, 61845, 61868, 61891, 61914, 61937, 61960, 61982, 62005,
    62028, 62050, 62073, 62095, 62118, 62140, 62162, 62184, 62206, 62229, 62250, 62272, 62294, 62316, 62338, 62359,
    62381, 62402, 62424, 62445, 62466, 62487, 62509, 62530, 62551, 62571, 62592, 62613, 62634, 62654, 62675, 62695,
    62716, 62736, 62757, 62777, 62797, 62817, 62837, 62857, 62877, 62896, 62916, 62936, 62955, 62975, 62994, 63014,
    63033, 63052, 63071, 63090, 63109, 63128, 63147, 63166, 63185, 63203, 63222, 63240, 63259, 63277, 63296, 63314,
    63332, 63350, 63368, 63386, 63404, 63422, 63439, 63457, 63474, 63492, 63509, 63527, 63544, 63561, 63578, 63595,
    63612, 63629, 63646, 63663, 63680, 63696, 63713, 63729, 63746, 63762, 63778, 63795, 63811, 63827, 63843, 63859,
    63874, 63890, 63906, 63921, 63937, 63952, 63968, 63983, 63998, 64014, 64029, 64044, 64059, 64073, 64088, 64103,
    64118, 64132, 64147, 64161, 64175, 64190, 64204, 64218, 64232, 64246, 64260, 64274, 64288, 64301, 64315, 64329,
    64342, 64355, 64369, 64382, 64395, 64408, 64421, 64434, 64447, 64460, 64473, 64485, 64498, 64510, 64523, 64535,
    64547, 64560, 64572, 64584, 64596, 64608, 64619, 64631, 64643, 64654, 64666, 64677, 64689, 64700, 64711, 64723,
    64734, 64745, 64756, 64766, 64777, 64788, 64798, 64809, 64819, 64830, 64840, 64850, 64861, 64871, 64881, 64891,
    64901, 64910, 64920, 64930, 64939, 64949, 64958, 64968, 64977, 64986, 64995, 65004, 65013, 65022, 65031, 65040,
    65048, 65057, 65065, 65074, 65082, 65090, 65099, 65107, 65115, 65123, 65131, 65139, 65146, 65154, 65162, 65169,
    65176, 65184, 65191, 65198, 65206, 65213, 65220, 65226, 65233, 65240, 65247, 65253, 65260, 65266, 65273, 65279,
    65285, 65291, 65297, 65303, 65309, 65315, 65321, 65327, 65332, 65338, 65343, 65349, 65354, 65359, 65364, 65369,
    65374, 65379, 65384, 65389, 65394, 65398, 65403, 65407, 65412, 65416, 65420, 65424, 65428, 65432, 65436, 65440,
    65444, 65448, 65451, 65455, 65458, 65462, 65465, 65468, 65471, 65474, 65477, 65480, 65483, 65486, 65489, 65491,
    65494, 65496, 65499, 65501, 65503, 65505, 65507, 65509, 65511, 65513, 65515, 65517, 65518, 65520, 65521, 65523,
    65524, 65525, 65526, 65527, 65528, 65529, 65530, 65531, 65532, 65532, 65533, 65533, 65534, 65534, 65534, 65534,
    65534, 65534, 65534, 65534, 65534, 65534, 65533, 65533, 65532, 65532, 65531, 65530, 65529, 65528, 65527, 65526,
    65525, 65524, 65523, 65521, 65520, 65518, 65517, 65515, 65513, 65511, 65509, 65507, 65505, 65503, 65501, 65499,
    65496, 65494, 65491, 65489, 65486, 65483, 65480, 65477, 65474, 65471, 65468, 65465, 65462, 65458, 65455, 65451,
    65448, 65444, 65440, 65436, 65432, 65428, 65424, 65420, 65416, 65412, 65407, 65403, 65398, 65394, 65389, 65384,
    65379, 65374, 65369, 65364, 65359, 65354, 65349, 65343, 65338, 65332, 65327, 65321, 65315, 65309, 65303, 65297,
    65291, 65285, 65279, 65273, 65266, 65260, 65253, 65247, 65240, 65233, 65226, 65220, 65213, 65206, 65198, 65191,
    65184, 65176, 65169, 65162, 65154, 65146, 65139, 65131, 65123, 65115, 65107, 65099, 65090, 65082, 65074, 65065,
    65057, 65048, 65040, 65031, 65022, 65013, 65004, 64995, 64986, 64977, 64968, 64958, 64949, 64939, 64930, 64920,
    64910, 64901, 64891, 64881, 64871, 64861, 64850, 64840, 64830, 64819, 64809, 64798, 64788, 64777, 64766, 64756,
    64745, 64734, 64723, 64711, 64700, 64689, 64677, 64666, 64654, 64643, 64631, 64619, 64608, 64596, 64584, 64572,
    64560, 64547, 64535, 64523, 64510, 64498, 64485, 64473, 64460, 64447, 64434, 64421, 64408, 64395, 64382, 64369,
    64355, 64342, 64329, 64315, 64301, 64288, 64274, 64260, 64246, 64232, 64218, 64204, 64190, 64175, 64161, 64147,
    64132, 64118, 64103, 64088, 64073, 64059, 64044, 64029, 64014, 63998, 63983, 63968, 63952, 63937, 63921, 63906,
    63890, 63874, 63859, 63843, 63827, 63811, 63795, 63778, 63762, 63746, 63729, 63713, 63696, 63680, 63663, 63646,
    63629, 63612, 63595, 63578, 63561, 63544, 63527, 63509, 63492, 63474, 63457, 63439, 63422, 63404, 63386, 63368,
    63350, 63332, 63314, 63296, 63277, 63259, 63240, 63222, 63203, 63185, 63166, 63147, 63128, 63109, 63090, 63071,
    63052, 63033, 63014, 62994, 62975, 62955, 62936, 62916, 62896, 62877, 62857, 62837, 62817, 62797, 62777, 62757,
    62736, 62716, 62695, 62675, 62654, 62634, 62613, 62592, 62571, 62551, 62530, 62509, 62487, 62466, 62445, 62424,
    62402, 62381, 62359, 62338, 62316, 62294, 62272, 62250, 62229, 62206, 62184, 62162, 62140, 62118, 62095, 62073,
    62050, 62028, 62005, 61982, 61960, 61937, 61914, 61891, 61868, 61845, 61821, 61798, 61775, 61751, 61728, 61704,
    61681, 61657, 61633, 61610, 61586, 61562, 61538, 61514, 61490, 61465, 61441, 61417, 61392, 61368, 61343, 61319,
    61294, 61269, 61244, 61220, 61195, 61170, 61144, 61119, 61094, 61069, 61043, 61018, 60993, 60967, 60941, 60916,
    60890, 60864, 60838, 60812, 60786, 60760, 60734, 60708, 60682, 60655, 60629, 60602, 60576, 60549, 60523, 60496,
    60469, 60442, 60415, 60388, 60361, 60334, 60307, 60280, 60252, 60225, 60197, 60170, 60142, 60115, 60087, 60059,
    60031, 60003, 59975, 59947, 59919, 59891, 59863, 59835, 59806, 59778, 59749, 59721, 59692, 59664, 59635, 59606,
    59577, 59548, 59519, 59490, 59461, 59432, 59403, 59374, 59344, 59315, 59285, 59256, 59226, 59197, 59167, 59137,
    59107, 59077, 59047, 59017, 58987, 58957, 58927, 58896, 58866, 58836, 58805, 58775, 58744, 58713, 58683, 58652,
    58621, 58590, 58559, 58528, 58497, 58466, 58435, 58404, 58372, 58341, 58309, 58278, 58246, 58215, 58183, 58151,
    58120, 58088, 58056, 58024, 57992, 57960, 57927, 57895, 57863, 57831, 57798, 57766, 57733, 57701, 57668, 57635,
    57603, 57570, 57537, 57504, 57471, 57438, 57405, 57372, 57339, 57305, 57272, 57239, 57205, 57172, 57138, 57105,
    57071, 57037, 57003, 56970, 56936, 56902, 56868, 56834, 56799, 56765, 56731, 56697, 56662, 56628, 56594, 56559,
    56524, 56490, 56455, 56420, 56386, 56351, 56316, 56281, 56246, 56211, 56176, 56140, 56105, 56070, 56035, 55999,
    55964, 55928, 55893, 55857, 55821, 55786, 55750, 55714, 55678, 55642, 55606, 55570, 55534, 55498, 55462, 55425,
    55389, 55353, 55316, 55280, 55243, 55207, 55170, 55133, 55097, 55060, 55023, 54986, 54949, 54912, 54875, 54838,
    54801, 54764, 54726, 54689, 54652, 54614, 54577, 54539, 54502, 54464, 54426, 54389, 54351, 54313, 54275, 54237,
    54199, 54161, 54123, 54085, 54047, 54009, 53970, 53932, 53894, 53855, 53817, 53778, 53740, 53701, 53662, 53624,
    53585, 53546, 53507, 53468, 53429, 53390, 53351, 53312, 53273, 53234, 53194, 53155, 53116, 53076, 53037, 52997,
    52958, 52918, 52879, 52839, 52799, 52759, 52720, 52680, 52640, 52600, 52560, 52520, 52480, 52439, 52399, 52359,
    52319, 52278, 52238, 52198, 52157, 52117, 52076, 52035, 51995, 51954, 51913, 51872, 51832, 51791, 51750, 51709,
    51668, 51627, 51586, 51545, 51503, 51462, 51421, 51379, 51338, 51297, 51255, 51214, 51172, 51131, 51089, 51047,
    51006, 50964, 50922, 50880, 50838, 50796, 50754, 50712, 50670, 50628, 50586, 50544, 50502, 50459, 50417, 50375,
    50332, 50290, 50247, 50205, 50162, 50120, 50077, 50034, 49992, 49949, 49906, 49863, 49820, 49778, 49735, 49692,
    49649, 49605, 49562, 49519, 49476, 49433, 49389, 49346, 49303, 49259, 49216, 49172, 49129, 49085, 49042, 48998,
    48955, 48911, 48867, 48823, 48780, 48736, 48692, 48648, 48604, 48560, 48516, 48472, 48428, 48384, 48339, 48295,
    48251, 48207, 48162, 48118, 48073, 48029, 47985, 47940, 47895, 47851, 47806, 47762, 47717, 47672, 47627, 47583,
    47538, 47493, 47448, 47403, 47358, 47313, 47268, 47223, 47178, 47133, 47088, 47042, 46997, 46952, 46907, 46861,
    46816, 46771, 46725, 46680, 46634, 46589, 46543, 46497, 46452, 46406, 46360, 46315, 46269, 46223, 46177, 46131,
    46086, 46040, 45994, 45948, 45902, 45856, 45810, 45763, 45717, 45671, 45625, 45579, 45533, 45486, 45440, 45394,
    45347, 45301, 45254, 45208, 45161, 45115, 45068, 45022, 44975, 44928, 44882, 44835, 44788, 44742, 44695, 44648,
    44601, 44554, 44507, 44461, 44414, 44367, 44320, 44273, 44226, 44178, 44131, 44084, 44037, 43990, 43943, 43895,
    43848, 43801, 43754, 43706, 43659, 43611, 43564, 43517, 43469, 43422, 43374, 43327, 43279, 43231, 43184, 43136,
    43088, 43041, 42993, 42945, 42897, 42850, 42802, 42754, 42706, 42658, 42610, 42562, 42515, 42467, 42419, 42371,
    42322, 42274, 42226, 42178, 42130, 42082, 42034, 41986, 41937, 41889, 41841, 41793, 41744, 41696, 41648, 41599,
    41551, 41502, 41454, 41405, 41357, 41308, 41260, 41211, 41163, 41114, 41066, 41017, 40968, 40920, 40871, 40822,
    40774, 40725, 40676, 40628, 40579, 40530, 40481, 40432, 40383, 40335, 40286, 40237, 40188, 40139, 40090, 40041,
    39992, 39943, 39894, 39845, 39796, 39747, 39698, 39649, 39599, 39550, 39501, 39452, 39403, 39354, 39304, 39255,
    39206, 39157, 39107, 39058, 39009, 38959, 38910, 38861, 38811, 38762, 38712, 38663, 38614, 38564, 38515, 38465,
    38416, 38366, 38317, 38267, 38218, 38168, 38119, 38069, 38019, 37970, 37920, 37871, 37821, 37771, 37722, 37672,
    37622, 37573, 37523, 37473, 37424, 37374, 37324, 37274, 37225, 37175, 37125, 37075, 37025, 36976, 36926, 36876,
    36826, 36776, 36726, 36676, 36627, 36577, 36527, 36477, 36427, 36377, 36327, 36277, 36227, 36177, 36127, 36077,
    36027, 35977, 35927, 35877, 35827, 35777, 35727, 35677, 35627, 35577, 35527, 35477, 35427, 35377, 35327, 35277,
    35226, 35176, 35126, 35076, 35026, 34976, 34926, 34876, 34825, 34775, 34725, 34675, 34625, 34575, 34525, 34474,
    34424, 34374, 34324, 34274, 34223, 34173, 34123, 34073, 34023, 33972, 33922, 33872, 33822, 33772, 33721, 33671,
    33621, 33571, 33521, 33470, 33420, 33370, 33320, 33269, 33219, 33169, 33119, 33068, 33018, 32968, 32918, 32867,
    32817, 32767, 32717, 32667, 32616, 32566, 32516, 32466, 32415, 32365, 32315, 32265, 32214, 32164, 32114, 32064,
    32013, 31963, 31913, 31863, 31813, 31762, 31712, 31662, 31612, 31562, 31511, 31461, 31411, 31361, 31311, 31260,
    31210, 31160, 31110, 31060, 31009, 30959, 30909, 30859, 30809, 30759, 30709, 30658, 30608, 30558, 30508, 30458,
    30408, 30358, 30308, 30257, 30207, 30157, 30107, 30057, 30007, 29957, 29907, 29857, 29807, 29757, 29707, 29657,
    29607, 29557, 29507, 29457, 29407, 29357, 29307, 29257, 29207, 29157, 29107, 29057, 29007, 28957, 28907, 28858,
    28808, 28758, 28708, 28658, 28608, 28558, 28509, 28459, 28409, 28359, 28309, 28260, 28210, 28160, 28110, 28061,
    28011, 27961, 27912, 27862, 27812, 27763, 27713, 27663, 27614, 27564, 27515, 27465, 27415, 27366, 27316, 27267,
    27217, 27168, 27118, 27069, 27019, 26970, 26920, 26871, 26822, 26772, 26723, 26673, 26624, 26575, 26525, 26476,
    26427, 26377, 26328, 26279, 26230, 26180, 26131, 26082, 26033, 25984, 25935, 25885, 25836, 25787, 25738, 25689,
    25640, 25591, 25542, 25493, 25444, 25395, 25346, 25297, 25248, 25199, 25151, 25102, 25053, 25004, 24955, 24906,
    24858, 24809, 24760, 24712, 24663, 24614, 24566, 24517, 24468, 24420, 24371, 24323, 24274, 24226, 24177, 24129,
    24080, 24032, 23983, 23935, 23886, 23838, 23790, 23741, 23693, 23645, 23597, 23548, 23500, 23452, 23404, 23356,
    23308, 23260, 23212, 23163, 23115, 23067, 23019, 22972, 22924, 22876, 22828, 22780, 22732, 22684, 22637, 22589,
    22541, 22493, 22446, 22398, 22350, 22303, 22255, 22207, 22160, 22112, 22065, 22017, 21970, 21923, 21875, 21828,
    21780, 21733, 21686, 21639, 21591, 21544, 21497, 21450, 21403, 21356, 21308, 21261, 21214, 21167, 21120, 21073,
    21027, 20980, 20933, 20886, 20839, 20792, 20746, 20699, 20652, 20606, 20559, 20512, 20466, 20419, 20373, 20326,
    20280, 20233, 20187, 20140, 20094, 20048, 20001, 19955, 19909, 19863, 19817, 19771, 19724, 19678, 19632, 19586,
    19540, 19494, 19448, 19403, 19357, 19311, 19265, 19219, 19174, 19128, 19082, 19037, 18991, 18945, 18900, 18854,
    18809, 18763, 18718, 18673, 18627, 18582, 18537, 18492, 18446, 18401, 18356, 18311, 18266, 18221, 18176, 18131,
    18086, 18041, 17996, 17951, 17907, 17862, 17817, 17772, 17728, 17683, 17639, 17594, 17549, 17505, 17461, 17416,
    17372, 17327, 17283, 17239, 17195, 17150, 17106, 17062, 17018, 16974, 16930, 16886, 16842, 16798, 16754, 16711,
    16667, 16623, 16579, 16536, 16492, 16449, 16405, 16362, 16318, 16275, 16231, 16188, 16145, 16101, 16058, 16015,
    15972, 15929, 15885, 15842, 15799, 15756, 15714, 15671, 15628, 15585, 15542, 15500, 15457, 15414, 15372, 15329,
    15287, 15244, 15202, 15159, 15117, 15075, 15032, 14990, 14948, 14906, 14864, 14822, 14780, 14738, 14696, 14654,
    14612, 14570, 14528, 14487, 14445, 14403, 14362, 14320, 14279, 14237, 14196, 14155, 14113, 14072, 14031, 13989,
    13948, 13907, 13866, 13825, 13784, 13743, 13702, 13662, 13621, 13580, 13539, 13499, 13458, 13417, 13377, 13336,
    13296, 13256, 13215, 13175, 13135, 13095, 13054, 13014, 12974, 12934, 12894, 12854, 12814, 12775, 12735, 12695,
    12655, 12616, 12576, 12537, 12497, 12458, 12418, 12379, 12340, 12300, 12261, 12222, 12183, 12144, 12105, 12066,
    12027, 11988, 11949, 11910, 11872, 11833, 11794, 11756, 11717, 11679, 11640, 11602, 11564, 11525, 11487, 11449,
    11411, 11373, 11335, 11297, 11259, 11221, 11183, 11145, 11108, 11070, 11032, 10995, 10957, 10920, 10882, 10845,
    10808, 10770, 10733, 10696, 10659, 10622, 10585, 10548, 10511, 10474, 10437, 10401, 10364, 10327, 10291, 10254,
    10218, 10181, 10145, 10109, 10072, 10036, 10000, 9964,  9928,  9892,  9856,  9820,  9784,  9748,  9713,  9677,
    9641,  9606,  9570,  9535,  9499,  9464,  9429,  9394,  9358,  9323,  9288,  9253,  9218,  9183,  9148,  9114,
    9079,  9044,  9010,  8975,  8940,  8906,  8872,  8837,  8803,  8769,  8735,  8700,  8666,  8632,  8598,  8564,
    8531,  8497,  8463,  8429,  8396,  8362,  8329,  8295,  8262,  8229,  8195,  8162,  8129,  8096,  8063,  8030,
    7997,  7964,  7931,  7899,  7866,  7833,  7801,  7768,  7736,  7703,  7671,  7639,  7607,  7574,  7542,  7510,
    7478,  7446,  7414,  7383,  7351,  7319,  7288,  7256,  7225,  7193,  7162,  7130,  7099,  7068,  7037,  7006,
    6975,  6944,  6913,  6882,  6851,  6821,  6790,  6759,  6729,  6698,  6668,  6638,  6607,  6577,  6547,  6517,
    6487,  6457,  6427,  6397,  6367,  6337,  6308,  6278,  6249,  6219,  6190,  6160,  6131,  6102,  6073,  6044,
    6015,  5986,  5957,  5928,  5899,  5870,  5842,  5813,  5785,  5756,  5728,  5699,  5671,  5643,  5615,  5587,
    5559,  5531,  5503,  5475,  5447,  5419,  5392,  5364,  5337,  5309,  5282,  5254,  5227,  5200,  5173,  5146,
    5119,  5092,  5065,  5038,  5011,  4985,  4958,  4932,  4905,  4879,  4852,  4826,  4800,  4774,  4748,  4722,
    4696,  4670,  4644,  4618,  4593,  4567,  4541,  4516,  4491,  4465,  4440,  4415,  4390,  4364,  4339,  4314,
    4290,  4265,  4240,  4215,  4191,  4166,  4142,  4117,  4093,  4069,  4044,  4020,  3996,  3972,  3948,  3924,
    3901,  3877,  3853,  3830,  3806,  3783,  3759,  3736,  3713,  3689,  3666,  3643,  3620,  3597,  3574,  3552,
    3529,  3506,  3484,  3461,  3439,  3416,  3394,  3372,  3350,  3328,  3305,  3284,  3262,  3240,  3218,  3196,
    3175,  3153,  3132,  3110,  3089,  3068,  3047,  3025,  3004,  2983,  2963,  2942,  2921,  2900,  2880,  2859,
    2839,  2818,  2798,  2777,  2757,  2737,  2717,  2697,  2677,  2657,  2638,  2618,  2598,  2579,  2559,  2540,
    2520,  2501,  2482,  2463,  2444,  2425,  2406,  2387,  2368,  2349,  2331,  2312,  2294,  2275,  2257,  2238,
    2220,  2202,  2184,  2166,  2148,  2130,  2112,  2095,  2077,  2060,  2042,  2025,  2007,  1990,  1973,  1956,
    1939,  1922,  1905,  1888,  1871,  1854,  1838,  1821,  1805,  1788,  1772,  1756,  1739,  1723,  1707,  1691,
    1675,  1660,  1644,  1628,  1613,  1597,  1582,  1566,  1551,  1536,  1520,  1505,  1490,  1475,  1461,  1446,
    1431,  1416,  1402,  1387,  1373,  1359,  1344,  1330,  1316,  1302,  1288,  1274,  1260,  1246,  1233,  1219,
    1205,  1192,  1179,  1165,  1152,  1139,  1126,  1113,  1100,  1087,  1074,  1061,  1049,  1036,  1024,  1011,
    999,   987,   974,   962,   950,   938,   926,   915,   903,   891,   880,   868,   857,   845,   834,   823,
    811,   800,   789,   778,   768,   757,   746,   736,   725,   715,   704,   694,   684,   673,   663,   653,
    643,   633,   624,   614,   604,   595,   585,   576,   566,   557,   548,   539,   530,   521,   512,   503,
    494,   486,   477,   469,   460,   452,   444,   435,   427,   419,   411,   403,   395,   388,   380,   372,
    365,   358,   350,   343,   336,   328,   321,   314,   308,   301,   294,   287,   281,   274,   268,   261,
    255,   249,   243,   237,   231,   225,   219,   213,   207,   202,   196,   191,   185,   180,   175,   170,
    165,   160,   155,   150,   145,   140,   136,   131,   127,   122,   118,   114,   110,   106,   102,   98,
    94,    90,    86,    83,    79,    76,    72,    69,    66,    63,    60,    57,    54,    51,    48,    45,
    43,    40,    38,    35,    33,    31,    29,    27,    25,    23,    21,    19,    17,    16,    14,    13,
    11,    10,    9,     8,     7,     6,     5,     4,     3,     2,     2,     1,     1,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     1,     1,     2,     2,     3,     4,     5,     6,     7,
    8,     9,     10,    11,    13,    14,    16,    17,    19,    21,    23,    25,    27,    29,    31,    33,
    35,    38,    40,    43,    45,    48,    51,    54,    57,    60,    63,    66,    69,    72,    76,    79,
    83,    86,    90,    94,    98,    102,   106,   110,   114,   118,   122,   127,   131,   136,   140,   145,
    150,   155,   160,   165,   170,   175,   180,   185,   191,   196,   202,   207,   213,   219,   225,   231,
    237,   243,   249,   255,   261,   268,   274,   281,   287,   294,   301,   308,   314,   321,   328,   336,
    343,   350,   358,   365,   372,   380,   388,   395,   403,   411,   419,   427,   435,   444,   452,   460,
    469,   477,   486,   494,   503,   512,   521,   530,   539,   548,   557,   566,   576,   585,   595,   604,
    614,   624,   633,   643,   653,   663,   673,   684,   694,   704,   715,   725,   736,   746,   757,   768,
    778,   789,   800,   811,   823,   834,   845,   857,   868,   880,   891,   903,   915,   926,   938,   950,
    962,   974,   987,   999,   1011,  1024,  1036,  1049,  1061,  1074,  1087,  1100,  1113,  1126,  1139,  1152,
    1165,  1179,  1192,  1205,  1219,  1233,  1246,  1260,  1274,  1288,  1302,  1316,  1330,  1344,  1359,  1373,
    1387,  1402,  1416,  1431,  1446,  1461,  1475,  1490,  1505,  1520,  1536,  1551,  1566,  1582,  1597,  1613,
    1628,  1644,  1660,  1675,  1691,  1707,  1723,  1739,  1756,  1772,  1788,  1805,  1821,  1838,  1854,  1871,
    1888,  1905,  1922,  1939,  1956,  1973,  1990,  2007,  2025,  2042,  2060,  2077,  2095,  2112,  2130,  2148,
    2166,  2184,  2202,  2220,  2238,  2257,  2275,  2294,  2312,  2331,  2349,  2368,  2387,  2406,  2425,  2444,
    2463,  2482,  2501,  2520,  2540,  2559,  2579,  2598,  2618,  2638,  2657,  2677,  2697,  2717,  2737,  2757,
    2777,  2798,  2818,  2839,  2859,  2880,  2900,  2921,  2942,  2963,  2983,  3004,  3025,  3047,  3068,  3089,
    3110,  3132,  3153,  3175,  3196,  3218,  3240,  3262,  3284,  3305,  3328,  3350,  3372,  3394,  3416,  3439,
    3461,  3484,  3506,  3529,  3552,  3574,  3597,  3620,  3643,  3666,  3689,  3713,  3736,  3759,  3783,  3806,
    3830,  3853,  3877,  3901,  3924,  3948,  3972,  3996,  4020,  4044,  4069,  4093,  4117,  4142,  4166,  4191,
    4215,  4240,  4265,  4290,  4314,  4339,  4364,  4390,  4415,  4440,  4465,  4491,  4516,  4541,  4567,  4593,
    4618,  4644,  4670,  4696,  4722,  4748,  4774,  4800,  4826,  4852,  4879,  4905,  4932,  4958,  4985,  5011,
    5038,  5065,  5092,  5119,  5146,  5173,  5200,  5227,  5254,  5282,  5309,  5337,  5364,  5392,  5419,  5447,
    5475,  5503,  5531,  5559,  5587,  5615,  5643,  5671,  5699,  5728,  5756,  5785,  5813,  5842,  5870,  5899,
    5928,  5957,  5986,  6015,  6044,  6073,  6102,  6131,  6160,  6190,  6219,  6249,  6278,  6308,  6337,  6367,
    6397,  6427,  6457,  6487,  6517,  6547,  6577,  6607,  6638,  6668,  6698,  6729,  6759,  6790,  6821,  6851,
    6882,  6913,  6944,  6975,  7006,  7037,  7068,  7099,  7130,  7162,  7193,  7225,  7256,  7288,  7319,  7351,
    7383,  7414,  7446,  7478,  7510,  7542,  7574,  7607,  7639,  7671,  7703,  7736,  7768,  7801,  7833,  7866,
    7899,  7931,  7964,  7997,  8030,  8063,  8096,  8129,  8162,  8195,  8229,  8262,  8295,  8329,  8362,  8396,
    8429,  8463,  8497,  8531,  8564,  8598,  8632,  8666,  8700,  8735,  8769,  8803,  8837,  8872,  8906,  8940,
    8975,  9010,  9044,  9079,  9114,  9148,  9183,  9218,  9253,  9288,  9323,  9358,  9394,  9429,  9464,  9499,
    9535,  9570,  9606,  9641,  9677,  9713,  9748,  9784,  9820,  9856,  9892,  9928,  9964,  10000, 10036, 10072,
    10109, 10145, 10181, 10218, 10254, 10291, 10327, 10364, 10401, 10437, 10474, 10511, 10548, 10585, 10622, 10659,
    10696, 10733, 10770, 10808, 10845, 10882, 10920, 10957, 10995, 11032, 11070, 11108, 11145, 11183, 11221, 11259,
    11297, 11335, 11373, 11411, 11449, 11487, 11525, 11564, 11602, 11640, 11679, 11717, 11756, 11794, 11833, 11872,
    11910, 11949, 11988, 12027, 12066, 12105, 12144, 12183, 12222, 12261, 12300, 12340, 12379, 12418, 12458, 12497,
    12537, 12576, 12616, 12655, 12695, 12735, 12775, 12814, 12854, 12894, 12934, 12974, 13014, 13054, 13095, 13135,
    13175, 13215, 13256, 13296, 13336, 13377, 13417, 13458, 13499, 13539, 13580, 13621, 13662, 13702, 13743, 13784,
    13825, 13866, 13907, 13948, 13989, 14031, 14072, 14113, 14155, 14196, 14237, 14279, 14320, 14362, 14403, 14445,
    14487, 14528, 14570, 14612, 14654, 14696, 14738, 14780, 14822, 14864, 14906, 14948, 14990, 15032, 15075, 15117,
    15159, 15202, 15244, 15287, 15329, 15372, 15414, 15457, 15500, 15542, 15585, 15628, 15671, 15714, 15756, 15799,
    15842, 15885, 15929, 15972, 16015, 16058, 16101, 16145, 16188, 16231, 16275, 16318, 16362, 16405, 16449, 16492,
    16536, 16579, 16623, 16667, 16711, 16754, 16798, 16842, 16886, 16930, 16974, 17018, 17062, 17106, 17150, 17195,
    17239, 17283, 17327, 17372, 17416, 17461, 17505, 17549, 17594, 17639, 17683, 17728, 17772, 17817, 17862, 17907,
    17951, 17996, 18041, 18086, 18131, 18176, 18221, 18266, 18311, 18356, 18401, 18446, 18492, 18537, 18582, 18627,
    18673, 18718, 18763, 18809, 18854, 18900, 18945, 18991, 19037, 19082, 19128, 19174, 19219, 19265, 19311, 19357,
    19403, 19448, 19494, 19540, 19586, 19632, 19678, 19724, 19771, 19817, 19863, 19909, 19955, 20001, 20048, 20094,
    20140, 20187, 20233, 20280, 20326, 20373, 20419, 20466, 20512, 20559, 20606, 20652, 20699, 20746, 20792, 20839,
    20886, 20933, 20980, 21027, 21073, 21120, 21167, 21214, 21261, 21308, 21356, 21403, 21450, 21497, 21544, 21591,
    21639, 21686, 21733, 21780, 21828, 21875, 21923, 21970, 22017, 22065, 22112, 22160, 22207, 22255, 22303, 22350,
    22398, 22446, 22493, 22541, 22589, 22637, 22684, 22732, 22780, 22828, 22876, 22924, 22972, 23019, 23067, 23115,
    23163, 23212, 23260, 23308, 23356, 23404, 23452, 23500, 23548, 23597, 23645, 23693, 23741, 23790, 23838, 23886,
    23935, 23983, 24032, 24080, 24129, 24177, 24226, 24274, 24323, 24371, 24420, 24468, 24517, 24566, 24614, 24663,
    24712, 24760, 24809, 24858, 24906, 24955, 25004, 25053, 25102, 25151, 25199, 25248, 25297, 25346, 25395, 25444,
    25493, 25542, 25591, 25640, 25689, 25738, 25787, 25836, 25885, 25935, 25984, 26033, 26082, 26131, 26180, 26230,
    26279, 26328, 26377, 26427, 26476, 26525, 26575, 26624, 26673, 26723, 26772, 26822, 26871, 26920, 26970, 27019,
    27069, 27118, 27168, 27217, 27267, 27316, 27366, 27415, 27465, 27515, 27564, 27614, 27663, 27713, 27763, 27812,
    27862, 27912, 27961, 28011, 28061, 28110, 28160, 28210, 28260, 28309, 28359, 28409, 28459, 28509, 28558, 28608,
    28658, 28708, 28758, 28808, 28858, 28907, 28957, 29007, 29057, 29107, 29157, 29207, 29257, 29307, 29357, 29407,
    29457, 29507, 29557, 29607, 29657, 29707, 29757, 29807, 29857, 29907, 29957, 30007, 30057, 30107, 30157, 30207,
    30257, 30308, 30358, 30408, 30458, 30508, 30558, 30608, 30658, 30709, 30759, 30809, 30859, 30909, 30959, 31009,
    31060, 31110, 31160, 31210, 31260, 31311, 31361, 31411, 31461, 31511, 31562, 31612, 31662, 31712, 31762, 31813,
    31863, 31913, 31963, 32013, 32064, 32114, 32164, 32214, 32265, 32315, 32365, 32415, 32466, 32516, 32566, 32616,
    32667, 32717,
};