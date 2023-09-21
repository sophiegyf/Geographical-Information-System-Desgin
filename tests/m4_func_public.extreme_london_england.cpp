#include <random>
#include <iostream>
#include <UnitTest++/UnitTest++.h>

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m3.h"
#include "m4.h"

#include "unit_test_util.h"
#include "courier_verify.h"

using ece297test::relative_error;
using ece297test::courier_path_is_legal;


SUITE(extreme_london_england_public) {
    TEST(extreme_london_england) {
        std::vector<DeliveryInf> deliveries;
        std::vector<IntersectionIdx> depots;
        float turn_penalty;
        std::vector<CourierSubPath> result_path;
        bool is_legal;

        deliveries = {DeliveryInf(290225, 138583), DeliveryInf(195025, 229928), DeliveryInf(333248, 169208), DeliveryInf(234918, 276299), DeliveryInf(201114, 196459), DeliveryInf(257378, 43885), DeliveryInf(253171, 148627), DeliveryInf(40132, 170980), DeliveryInf(55576, 313916), DeliveryInf(212828, 19174), DeliveryInf(188995, 136561), DeliveryInf(111610, 240002), DeliveryInf(144383, 304956), DeliveryInf(69332, 17888), DeliveryInf(19016, 297628), DeliveryInf(28077, 298138), DeliveryInf(207024, 309267), DeliveryInf(291961, 195251), DeliveryInf(25326, 104446), DeliveryInf(125304, 332622), DeliveryInf(313352, 104484), DeliveryInf(121141, 299319), DeliveryInf(48055, 156127), DeliveryInf(256584, 133881), DeliveryInf(250014, 334939), DeliveryInf(213033, 59917), DeliveryInf(93828, 34291), DeliveryInf(61702, 237860), DeliveryInf(285266, 2735), DeliveryInf(208716, 101958), DeliveryInf(2082, 300903), DeliveryInf(322102, 23196), DeliveryInf(204987, 197567), DeliveryInf(314661, 51291), DeliveryInf(53297, 138004), DeliveryInf(105073, 67260), DeliveryInf(317554, 51285), DeliveryInf(272821, 264582), DeliveryInf(120815, 219066), DeliveryInf(162925, 11580), DeliveryInf(70063, 74017), DeliveryInf(109448, 125896), DeliveryInf(91964, 261902), DeliveryInf(317882, 327516), DeliveryInf(187918, 109137), DeliveryInf(56072, 234942), DeliveryInf(295985, 150016), DeliveryInf(236519, 216063), DeliveryInf(346005, 307326), DeliveryInf(230309, 281446), DeliveryInf(29404, 226699), DeliveryInf(67089, 265452), DeliveryInf(115467, 312308), DeliveryInf(270507, 53197), DeliveryInf(308410, 105042), DeliveryInf(164459, 91739), DeliveryInf(276865, 115894), DeliveryInf(186145, 256366), DeliveryInf(135834, 252122), DeliveryInf(230848, 188128), DeliveryInf(51937, 186397), DeliveryInf(53886, 24805), DeliveryInf(311324, 81710), DeliveryInf(329547, 137), DeliveryInf(122541, 312071), DeliveryInf(20493, 20749), DeliveryInf(273213, 193156), DeliveryInf(33275, 229690), DeliveryInf(26630, 11925), DeliveryInf(94888, 148680), DeliveryInf(84983, 204561), DeliveryInf(50519, 82188), DeliveryInf(150712, 272765), DeliveryInf(209650, 78709), DeliveryInf(104427, 298743), DeliveryInf(18706, 75926), DeliveryInf(127582, 24357), DeliveryInf(51723, 258140), DeliveryInf(95769, 95688), DeliveryInf(276527, 88550), DeliveryInf(72059, 55491), DeliveryInf(249571, 283559), DeliveryInf(157327, 195655), DeliveryInf(19838, 28436), DeliveryInf(279997, 323842), DeliveryInf(73778, 91041), DeliveryInf(317716, 163030), DeliveryInf(186374, 293742), DeliveryInf(78075, 297480), DeliveryInf(13879, 239024), DeliveryInf(340100, 197478), DeliveryInf(199094, 249450), DeliveryInf(118361, 228889), DeliveryInf(269898, 1561), DeliveryInf(176685, 49291), DeliveryInf(97410, 226138), DeliveryInf(325003, 121965), DeliveryInf(9093, 200895), DeliveryInf(32559, 156906), DeliveryInf(220783, 84330)};
        depots = {55};
        turn_penalty = 30.000000000;
        {
        	ECE297_TIME_CONSTRAINT(50000);
        	
        	result_path = travelingCourier(deliveries, depots, turn_penalty);
        }

        is_legal = courier_path_is_legal(deliveries, depots, result_path);
        CHECK(is_legal);

        if(is_legal) {
        	double path_cost = ece297test::compute_courier_path_travel_time(result_path, turn_penalty);
        	std::cout << "QoR extreme_london_england: " << path_cost << std::endl;
        } else {
        	std::cout << "QoR extreme_london_england: INVALID" << std::endl;
        }

    } //extreme_london_england

} //extreme_london_england_public

