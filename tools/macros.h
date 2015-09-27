#ifndef EMP_MACROS_H
#define EMP_MACROS_H

//////////////////////////////////////////////////////////////////////////////////////////
//
//  Generally useful macros that can perform cools tricks.  As with all macros, use only
//  after careful exclusion of alternative approaches!
//
//  The Macros to highlight here are:
//
//  ===== String Handling and Printing =====
//  EMP_PRINT_RESULT(A) will print to std::cout both the string that was passed into the
//       macro and what that string evaluates to.
//
//  EMP_STRINGIFY(...) converts all arguments into a single string (including commas).
//  EMP_STRINGIFY_EACH(...) converts each argument into a string (leaving commas).
//
//  ===== Managing variadic arguments =====
//  EMP_COUNT_ARGS(...) returns the number of arguments in the __VA_ARGS__
//  EMP_GET_ARG(N, ...) return the Nth arg that follows
//  EMP_POP_ARGS(N, ...) remove the first N args, return the rest.
//  EMP_PACK_ARGS(...) Group args together in parens so they are treated like one argument.
//  EMP_DUPLICATE_ARGS(N, ...) makes N collated copies of all args to follow.
//  EMP_CROP_ARGS_TO(N, ...) reduces N args (must have at least that many)
//  EMP_FORCE_ARGS_TO(N, P, ...) Crops or pads (with p) args to be exactly N long.
//  EMP_ROTATE_ARGS(...) Moves the first argument to the end of the arg list.
//  EMP_SELECT_ARGS(PATTERN, ...) Uses the (repeating) patter to select arguments to keep.
//    Pattern is an argument pack consisting of i (include) or x (exclude). 
//  EMP_GET_ODD_ARGS(...) will return all arguments at odd positions (1,3,5,7, etc.)
//  EMP_GET_EVEN_ARGS(...) will return all arguments at odd positions (2,4,6,8, etc.)
//  EMP_REVERSE_ARGS(...) Reverse the order of arguments passed in.
//
//  ===== Argument Manipulation and Formatting =====
//  EMP_MERGE(...) merge all arguments (after conversion) into a single unit.
//  EMP_WRAP_EACH(W, ...) will run macro W on each of the other args and concatinate them.
//  EMP_LAYOUT(W, P, ...) Similar to EMP_WRAP_EACH, but puts a P between each arg pair.
//  EMP_WRAP_ARGS(W, ...) Similar to EMP_WRAP_EACH, but puts a COMMA between each arg pair.
//  EMP_WRAP_ARG_PAIRS(W, ...) Similar to EMP_WRAP_ARGS, but passes pairs of args into W.
//
//  ===== Macro Building =====
//  EMP_ASSEMBLE_MACRO takes in a prefix and set of arguments and appends the size of the
//  number of arguments to the prefix, and passes in all of the arguments.
//
//  EMP_ASSEMBLE_MACRO_1ARG assumes the first argument after the prefix should not count
//  toward the size, but passed in anyway. (*_?ARG  works for more arguments up to 10).
//
//  EMP_FAKE_ARG or EMP_FAKE_2ARG behave as a single argument.  If, in manipulating them
//  You make them become EMP_CONVERT_ARG_EMP_FAKE_ARG(A) or EMP_CONVERT_ARG_EMP_FAKE_2ARG(A)
//  (i.e., prepend with EMP_CONVERT and provide an argument) it will trigger a conversion.
//  If you prepend anything else similarly, it wil NOT trigger a conversion.
//  
//  This is especially useful with _2ARG since anything unconverted will be a single
//  argument, while anything converted will be two, allowing us to shift arguments
//  to perform conditional behaviors.
//
//
//  Development Notes:
//  * We need to standardize how we handle macros that covert inputs to comma-separated
//    results vs those that merge them together.  One option is to have comma-separated the
//    default and then have an EMP_REMOVE_COMMAS (or somesuch)
//  * EMP_TYPES_TO_ARGS (not yet list above) is poorly named.  Maybe EMP_DECLARE_ARGS?
//  * It would be useful to have EMP_WRAP_WITH_ID which passes in the position ID as the
//    second argument.  This would allow us to, for example, streamline EMP_TYPES_TO_ARGS.
//
//  * A more generic EMP_WRAP macro that is specified on the fly.  For example:
//      EMP_WRAP(W,2,4,A,B,a,b,c,d,e,f,g,h,i,j,k,l)
//    would assume six args in each wrap, A, B, and the rest broken into groups of four. I.e.:
//      W(A,B,a,b,c,d), W(A,B,e,f,g,h), W(A,B,i,j,k,l)
//

#include "macro_math.h"

// The below values allow you to have EMP_FAKE_ARG or EMP_FAKE_2ARG as a single argument.
// If you prepend it with EMP_CONVERT it will trigger a conversion.  If you prepend anything
// else similarly, it wil NOT triggera a conversion (and stay a single argument)
#define EMP_CONVERT_ARG_EMP_FAKE_ARG(A) A
#define EMP_CONVERT_ARG_EMP_FAKE_2ARG(A) ~, A


// EMP_STRINGIFY takes any input, processes macros, and puts the result in quotes.
#define EMP_STRINGIFY(...) EMP_STRINGIFY_IMPL(__VA_ARGS__)
#define EMP_STRINGIFY_IMPL(...) #__VA_ARGS__

#define EMP_PRINT_RESULT_IMPL(STREAM, LHS, RHS) STREAM << "[[" << LHS << "]] = [[" << RHS << "]]" << std::endl
#define EMP_PRINT_RESULT_TO(STREAM, A) EMP_PRINT_RESULT_IMPL(STREAM, #A, A)
#define EMP_PRINT_RESULT(A) EMP_PRINT_RESULT_IMPL(std::cout, #A, A)

#define EMP_POP_ARGS_IMPL_1(A,...) __VA_ARGS__
#define EMP_POP_ARGS_IMPL_2(A,B,...) __VA_ARGS__
#define EMP_POP_ARGS_IMPL_4(A,B,C,D,...) __VA_ARGS__
#define EMP_POP_ARGS_IMPL_8(A,B,C,D,E,F,G,H,...) __VA_ARGS__
#define EMP_POP_ARGS_IMPL_16(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,...) __VA_ARGS__
#define EMP_POP_ARGS_IMPL_32(...)  EMP_POP_ARGS_16(EMP_POP_ARGS_16(__VA_ARGS__) )
#define EMP_POP_ARGS_IMPL_64(...)  EMP_POP_ARGS_32(EMP_POP_ARGS_32(__VA_ARGS__) )
#define EMP_POP_ARGS_IMPL_128(...) EMP_POP_ARGS_64(EMP_POP_ARGS_64(__VA_ARGS__) )
#define EMP_POP_ARGS_IMPL_256(...) EMP_POP_ARGS_128(EMP_POP_ARGS_128(__VA_ARGS__) )
#define EMP_POP_ARGS_IMPL_512(...) EMP_POP_ARGS_256(EMP_POP_ARGS_256(__VA_ARGS__) )

#define EMP_POP_ARGS_0(...) __VA_ARGS__
#define EMP_POP_ARGS_1(...) EMP_POP_ARGS_IMPL_1(__VA_ARGS__)
#define EMP_POP_ARGS_2(...) EMP_POP_ARGS_IMPL_2(__VA_ARGS__)
#define EMP_POP_ARGS_4(...) EMP_POP_ARGS_IMPL_4(__VA_ARGS__)
#define EMP_POP_ARGS_8(...) EMP_POP_ARGS_IMPL_8(__VA_ARGS__)
#define EMP_POP_ARGS_16(...) EMP_POP_ARGS_IMPL_16(__VA_ARGS__)
#define EMP_POP_ARGS_32(...) EMP_POP_ARGS_IMPL_32(__VA_ARGS__)
#define EMP_POP_ARGS_64(...) EMP_POP_ARGS_IMPL_64(__VA_ARGS__)
#define EMP_POP_ARGS_128(...) EMP_POP_ARGS_IMPL_128(__VA_ARGS__)
#define EMP_POP_ARGS_256(...) EMP_POP_ARGS_IMPL_256(__VA_ARGS__)
#define EMP_POP_ARGS_512(...) EMP_POP_ARGS_IMPL_512(__VA_ARGS__)

#define EMP_POP_ARG(...) EMP_POP_ARGS_IMPL_1(__VA_ARGS__)
#define EMP_POP_ARGS(N, ...) EMP_EVAL( EMP_POP_ARGS_SUM EMP_EMPTY() (EMP_EVAL(EMP_DEC_TO_SUM(N)),__VA_ARGS__) )
#define EMP_POP_ARGS_SUM(S0,S1,S2,S3,S4,S5,S6,S7,S8,S9,...) EMP_POP_ARGS_##S0( EMP_POP_ARGS_##S1( EMP_POP_ARGS_##S2( EMP_POP_ARGS_##S3( EMP_POP_ARGS_##S4( EMP_POP_ARGS_##S5( EMP_POP_ARGS_##S6( EMP_POP_ARGS_##S7( EMP_POP_ARGS_##S8( EMP_POP_ARGS_##S9( __VA_ARGS__ ))))))))))

#define EMP_GET_ARG(N, ...) EMP_GET_ARG_1( EMP_POP_ARGS( EMP_DEC(N), __VA_ARGS__ ) )
#define EMP_GET_ARG_1(...) EMP_GET_ARG_1_IMPL(__VA_ARGS__)
#define EMP_GET_ARG_1_IMPL(A, ...) A
#define EMP_GET_ARG_2(...) EMP_GET_ARG_2_IMPL(__VA_ARGS__)
#define EMP_GET_ARG_2_IMPL(A, B, ...) B

#define EMP_COUNT_ARGS(...) EMP_GET_ARG(1023, __VA_ARGS__, 1022, 1021, 1020, 1019, 1018, 1017, 1016, 1015, 1014, 1013, 1012, 1011, 1010, 1009, 1008, 1007, 1006, 1005, 1004, 1003, 1002, 1001, 1000, 999, 998, 997, 996, 995, 994, 993, 992, 991, 990, 989, 988, 987, 986, 985, 984, 983, 982, 981, 980, 979, 978, 977, 976, 975, 974, 973, 972, 971, 970, 969, 968, 967, 966, 965, 964, 963, 962, 961, 960, 959, 958, 957, 956, 955, 954, 953, 952, 951, 950, 949, 948, 947, 946, 945, 944, 943, 942, 941, 940, 939, 938, 937, 936, 935, 934, 933, 932, 931, 930, 929, 928, 927, 926, 925, 924, 923, 922, 921, 920, 919, 918, 917, 916, 915, 914, 913, 912, 911, 910, 909, 908, 907, 906, 905, 904, 903, 902, 901, 900, 899, 898, 897, 896, 895, 894, 893, 892, 891, 890, 889, 888, 887, 886, 885, 884, 883, 882, 881, 880, 879, 878, 877, 876, 875, 874, 873, 872, 871, 870, 869, 868, 867, 866, 865, 864, 863, 862, 861, 860, 859, 858, 857, 856, 855, 854, 853, 852, 851, 850, 849, 848, 847, 846, 845, 844, 843, 842, 841, 840, 839, 838, 837, 836, 835, 834, 833, 832, 831, 830, 829, 828, 827, 826, 825, 824, 823, 822, 821, 820, 819, 818, 817, 816, 815, 814, 813, 812, 811, 810, 809, 808, 807, 806, 805, 804, 803, 802, 801, 800, 799, 798, 797, 796, 795, 794, 793, 792, 791, 790, 789, 788, 787, 786, 785, 784, 783, 782, 781, 780, 779, 778, 777, 776, 775, 774, 773, 772, 771, 770, 769, 768, 767, 766, 765, 764, 763, 762, 761, 760, 759, 758, 757, 756, 755, 754, 753, 752, 751, 750, 749, 748, 747, 746, 745, 744, 743, 742, 741, 740, 739, 738, 737, 736, 735, 734, 733, 732, 731, 730, 729, 728, 727, 726, 725, 724, 723, 722, 721, 720, 719, 718, 717, 716, 715, 714, 713, 712, 711, 710, 709, 708, 707, 706, 705, 704, 703, 702, 701, 700, 699, 698, 697, 696, 695, 694, 693, 692, 691, 690, 689, 688, 687, 686, 685, 684, 683, 682, 681, 680, 679, 678, 677, 676, 675, 674, 673, 672, 671, 670, 669, 668, 667, 666, 665, 664, 663, 662, 661, 660, 659, 658, 657, 656, 655, 654, 653, 652, 651, 650, 649, 648, 647, 646, 645, 644, 643, 642, 641, 640, 639, 638, 637, 636, 635, 634, 633, 632, 631, 630, 629, 628, 627, 626, 625, 624, 623, 622, 621, 620, 619, 618, 617, 616, 615, 614, 613, 612, 611, 610, 609, 608, 607, 606, 605, 604, 603, 602, 601, 600, 599, 598, 597, 596, 595, 594, 593, 592, 591, 590, 589, 588, 587, 586, 585, 584, 583, 582, 581, 580, 579, 578, 577, 576, 575, 574, 573, 572, 571, 570, 569, 568, 567, 566, 565, 564, 563, 562, 561, 560, 559, 558, 557, 556, 555, 554, 553, 552, 551, 550, 549, 548, 547, 546, 545, 544, 543, 542, 541, 540, 539, 538, 537, 536, 535, 534, 533, 532, 531, 530, 529, 528, 527, 526, 525, 524, 523, 522, 521, 520, 519, 518, 517, 516, 515, 514, 513, 512, 511, 510, 509, 508, 507, 506, 505, 504, 503, 502, 501, 500, 499, 498, 497, 496, 495, 494, 493, 492, 491, 490, 489, 488, 487, 486, 485, 484, 483, 482, 481, 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446, 445, 444, 443, 442, 441, 440, 439, 438, 437, 436, 435, 434, 433, 432, 431, 430, 429, 428, 427, 426, 425, 424, 423, 422, 421, 420, 419, 418, 417, 416, 415, 414, 413, 412, 411, 410, 409, 408, 407, 406, 405, 404, 403, 402, 401, 400, 399, 398, 397, 396, 395, 394, 393, 392, 391, 390, 389, 388, 387, 386, 385, 384, 383, 382, 381, 380, 379, 378, 377, 376, 375, 374, 373, 372, 371, 370, 369, 368, 367, 366, 365, 364, 363, 362, 361, 360, 359, 358, 357, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 343, 342, 341, 340, 339, 338, 337, 336, 335, 334, 333, 332, 331, 330, 329, 328, 327, 326, 325, 324, 323, 322, 321, 320, 319, 318, 317, 316, 315, 314, 313, 312, 311, 310, 309, 308, 307, 306, 305, 304, 303, 302, 301, 300, 299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 284, 283, 282, 281, 280, 279, 278, 277, 276, 275, 274, 273, 272, 271, 270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 259, 258, 257, 256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define EMP_dup_bin_0(...)
#define EMP_dup_bin_1(...) , __VA_ARGS__
#define EMP_dup_bin_2(...) , __VA_ARGS__, __VA_ARGS__
#define EMP_dup_bin_4(...) , __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__
#define EMP_dup_bin_8(...)   EMP_dup_bin_4(__VA_ARGS__)EMP_dup_bin_4(__VA_ARGS__)
#define EMP_dup_bin_16(...)  EMP_dup_bin_8(__VA_ARGS__)EMP_dup_bin_8(__VA_ARGS__)
#define EMP_dup_bin_32(...)  EMP_dup_bin_16(__VA_ARGS__)EMP_dup_bin_16(__VA_ARGS__)
#define EMP_dup_bin_64(...)  EMP_dup_bin_32(__VA_ARGS__)EMP_dup_bin_32(__VA_ARGS__)
#define EMP_dup_bin_128(...) EMP_dup_bin_64(__VA_ARGS__)EMP_dup_bin_64(__VA_ARGS__)
#define EMP_dup_bin_256(...) EMP_dup_bin_128(__VA_ARGS__)EMP_dup_bin_128(__VA_ARGS__)
#define EMP_dup_bin_512(...) EMP_dup_bin_256(__VA_ARGS__)EMP_dup_bin_256(__VA_ARGS__)

#define EMP_DUPLICATE_ARGS(N, ...) EMP_DUPLICATE_ARGS_SUM EMP_EMPTY() (EMP_EVAL(EMP_DEC_TO_SUM(N)),__VA_ARGS__)
#define EMP_DUPLICATE_ARGS_SUM(S0,S1,S2,S3,S4,S5,S6,S7,S8,S9,...) EMP_POP_ARG(~ EMP_dup_bin_##S0(__VA_ARGS__)EMP_dup_bin_##S1(__VA_ARGS__)EMP_dup_bin_##S2(__VA_ARGS__)EMP_dup_bin_##S3(__VA_ARGS__)EMP_dup_bin_##S4(__VA_ARGS__)EMP_dup_bin_##S5(__VA_ARGS__)EMP_dup_bin_##S6(__VA_ARGS__)EMP_dup_bin_##S7(__VA_ARGS__)EMP_dup_bin_##S8(__VA_ARGS__)EMP_dup_bin_##S9(__VA_ARGS__))


// Some PARAMETER PACK manipulatation (for macros, sets of args in parens to they are treated
// as if they were a single argument.

#define EMP_ECHO_ARGS(...) __VA_ARGS__
#define EMP_PACK_ARGS(...) (__VA_ARGS__)
#define EMP_UNPACK_ARGS(A) EMP_ECHO_ARGS A

#define EMP_PACK_POP(PACK) (EMP_POP_ARG PACK)
#define EMP_PACK_TOP(PACK) EMP_GET_ARG_1 PACK
#define EMP_PACK_PUSH(NEW, PACK) (NEW,EMP_UNPACK_ARGS(PACK))
#define EMP_PACK_PUSH_REAR(NEW, PACK) (EMP_UNPACK_ARGS(PACK),NEW)
#define EMP_PACK_SIZE(PACK) EMP_COUNT_ARGS PACK

// C is the CALL needed to be made on each parameter
// F is a pack of FIXED parameters sent to all calls.
// N is the NEXT call count needed to be done.
// P is the pack of call counts the still need to be done
// A is the number of arguments in P.
#define EMP_CALL_BY_PACKS(C, F, ...)                                     \
  EMP_CALL_BY_PACKS_impl(C, F, EMP_DEC_TO_PACK(EMP_COUNT_ARGS(__VA_ARGS__)), __VA_ARGS__ )
// EMP_CALL_BY_PACKS_impl(C, F, EMP_DEC_TO_PACK(EMP_DEC(EMP_COUNT_ARGS(__VA_ARGS__))), __VA_ARGS__ )
#define EMP_CALL_BY_PACKS_impl(C, F, P, ...) \
  EMP_CALL_BY_PACKS_implB(C, F, EMP_PACK_SIZE(P), EMP_PACK_PUSH_REAR(~, P), __VA_ARGS__)
#define EMP_CALL_BY_PACKS_implB(C, F, A, P, ...) EMP_CALL_BY_PACKS_implC(C, F, A, P, __VA_ARGS__)
#define EMP_CALL_BY_PACKS_implC(C, F, A, P, ...) \
  EMP_CALL_BY_PACKS_implD(C, EMP_PACK_TOP(P), F, A, EMP_PACK_POP(P), __VA_ARGS__)
#define EMP_CALL_BY_PACKS_implD(C, N, F, A, P, ...)                      \
  EMP_CALL_BY_PACKS_impl ## A(C, N, F, P, __VA_ARGS__)

#define EMP_CALL_BY_PACKS_impl1(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)
#define EMP_CALL_BY_PACKS_impl2(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl1(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl3(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl2(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl4(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl3(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl5(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl4(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl6(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl5(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl7(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl6(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl8(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl7(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl9(C, N, F, P, ...)                        \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl8(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )
#define EMP_CALL_BY_PACKS_impl10(C, N, F, P, ...)                       \
  EMP_CALL_BY_PACKS_do_call(C, N, F, __VA_ARGS__)                       \
  EMP_CALL_BY_PACKS_impl9(C, EMP_PACK_TOP(P), F, EMP_PACK_POP(P), EMP_POP_ARGS_ ## N(__VA_ARGS__) )

#define EMP_CALL_BY_PACKS_do_call(C, V, F, ...) C ## V(F, __VA_ARGS__)


// Replace all of the commas in an argument set with something else (including nothing)
#define EMP_REPLACE_COMMAS(X, ...) \
  EMP_REPLACE_COMMAS_pack(EMP_DEC_TO_PACK( EMP_DEC(EMP_COUNT_ARGS(__VA_ARGS__)) ), X, __VA_ARGS__)
#define EMP_REPLACE_COMMAS_pack(P, X, ...) EMP_PACK_SIZE(P)   @CAO

#define EMP_REPLACE_COMMAS_1(X, A, ...) A
#define EMP_REPLACE_COMMAS_2(X, A,B, ...) A X B
#define EMP_REPLACE_COMMAS_4(X, A,B,C,D, ...) A X B X C X D
#define EMP_REPLACE_COMMAS_8(X, A,B,C,D,E,F,G,H, ...) A X B X C X D X E X F X G X H
#define EMP_REPLACE_COMMAS_16(X, ...) \
  EMP_REPLACE_COMMAS_8(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_8(__VA_ARGS__) )
#define EMP_REPLACE_COMMAS_32(X, ...) \
  EMP_REPLACE_COMMAS_16(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_16(__VA_ARGS__) )
#define EMP_REPLACE_COMMAS_64(X, ...) \
  EMP_REPLACE_COMMAS_32(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_32(__VA_ARGS__) )
#define EMP_REPLACE_COMMAS_128(X, ...) \
  EMP_REPLACE_COMMAS_64(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_64(__VA_ARGS__) )
#define EMP_REPLACE_COMMAS_256(X, ...) \
  EMP_REPLACE_COMMAS_128(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_128(__VA_ARGS__) )
#define EMP_REPLACE_COMMAS_512(X, ...) \
  EMP_REPLACE_COMMAS_256(__VA_ARGS__) X EMP_REPLACE_COMMAS( EMP_POP_ARGS_256(__VA_ARGS__) )

// Save the first N args (assumes at least N args exist!
#define EMP_CROP_ARGS_TO(N, ...) EMP_CROP_ARGS_TO_ ## N (__VA_ARGS__, ~)
#define EMP_CROP_ARGS_TO_1(A, ...) A
#define EMP_CROP_ARGS_TO_2(A, B, ...) A, B
#define EMP_CROP_ARGS_TO_3(A, B, C, ...) A, B, C
#define EMP_CROP_ARGS_TO_4(A, B, C, D, ...) A, B, C, D
#define EMP_CROP_ARGS_TO_5(A, B, C, D, E, ...) A, B, C, D, E
#define EMP_CROP_ARGS_TO_6(A, B, C, D, E, F, ...) A, B, C, D, E, F
#define EMP_CROP_ARGS_TO_7(A, ...) A, EMP_CROP_ARGS_TO_6(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_8(A, ...) A, EMP_CROP_ARGS_TO_7(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_9(A, ...) A, EMP_CROP_ARGS_TO_8(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_10(A, ...) A, EMP_CROP_ARGS_TO_9(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_11(A, ...) A, EMP_CROP_ARGS_TO_10(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_12(A, ...) A, EMP_CROP_ARGS_TO_11(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_13(A, ...) A, EMP_CROP_ARGS_TO_12(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_14(A, ...) A, EMP_CROP_ARGS_TO_13(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_15(A, ...) A, EMP_CROP_ARGS_TO_14(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_16(A, ...) A, EMP_CROP_ARGS_TO_15(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_17(A, ...) A, EMP_CROP_ARGS_TO_16(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_18(A, ...) A, EMP_CROP_ARGS_TO_17(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_19(A, ...) A, EMP_CROP_ARGS_TO_18(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_20(A, ...) A, EMP_CROP_ARGS_TO_19(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_21(A, ...) A, EMP_CROP_ARGS_TO_20(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_22(A, ...) A, EMP_CROP_ARGS_TO_21(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_23(A, ...) A, EMP_CROP_ARGS_TO_22(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_24(A, ...) A, EMP_CROP_ARGS_TO_23(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_25(A, ...) A, EMP_CROP_ARGS_TO_24(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_26(A, ...) A, EMP_CROP_ARGS_TO_25(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_27(A, ...) A, EMP_CROP_ARGS_TO_26(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_28(A, ...) A, EMP_CROP_ARGS_TO_27(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_29(A, ...) A, EMP_CROP_ARGS_TO_28(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_30(A, ...) A, EMP_CROP_ARGS_TO_29(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_31(A, ...) A, EMP_CROP_ARGS_TO_30(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_32(A, ...) A, EMP_CROP_ARGS_TO_31(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_33(A, ...) A, EMP_CROP_ARGS_TO_32(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_34(A, ...) A, EMP_CROP_ARGS_TO_33(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_35(A, ...) A, EMP_CROP_ARGS_TO_34(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_36(A, ...) A, EMP_CROP_ARGS_TO_35(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_37(A, ...) A, EMP_CROP_ARGS_TO_36(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_38(A, ...) A, EMP_CROP_ARGS_TO_37(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_39(A, ...) A, EMP_CROP_ARGS_TO_38(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_40(A, ...) A, EMP_CROP_ARGS_TO_39(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_41(A, ...) A, EMP_CROP_ARGS_TO_40(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_42(A, ...) A, EMP_CROP_ARGS_TO_41(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_43(A, ...) A, EMP_CROP_ARGS_TO_42(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_44(A, ...) A, EMP_CROP_ARGS_TO_43(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_45(A, ...) A, EMP_CROP_ARGS_TO_44(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_46(A, ...) A, EMP_CROP_ARGS_TO_45(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_47(A, ...) A, EMP_CROP_ARGS_TO_46(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_48(A, ...) A, EMP_CROP_ARGS_TO_47(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_49(A, ...) A, EMP_CROP_ARGS_TO_48(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_50(A, ...) A, EMP_CROP_ARGS_TO_49(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_51(A, ...) A, EMP_CROP_ARGS_TO_50(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_52(A, ...) A, EMP_CROP_ARGS_TO_51(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_53(A, ...) A, EMP_CROP_ARGS_TO_52(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_54(A, ...) A, EMP_CROP_ARGS_TO_53(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_55(A, ...) A, EMP_CROP_ARGS_TO_54(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_56(A, ...) A, EMP_CROP_ARGS_TO_55(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_57(A, ...) A, EMP_CROP_ARGS_TO_56(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_58(A, ...) A, EMP_CROP_ARGS_TO_57(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_59(A, ...) A, EMP_CROP_ARGS_TO_58(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_60(A, ...) A, EMP_CROP_ARGS_TO_59(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_61(A, ...) A, EMP_CROP_ARGS_TO_60(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_62(A, ...) A, EMP_CROP_ARGS_TO_61(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_63(A, ...) A, EMP_CROP_ARGS_TO_62(__VA_ARGS__)
#define EMP_CROP_ARGS_TO_64(A, ...) A, EMP_CROP_ARGS_TO_63(__VA_ARGS__)

// Force arguments to a specific number.  If less, pad them; if more crop them.
#define EMP_FORCE_ARGS_TO(N, PAD, ...) EMP_CROP_ARGS_TO(N, __VA_ARGS__, EMP_DUPLICATE_ARGS(N, PAD))

#define EMP_ROTATE_ARGS(A, ...) __VA_ARGS__, A
#define EMP_RUN_JOIN(A, B) A B


// A generic technique to trim the arguments we have.  In parens, list i or x for each
// position and whether it should be included or excluded.  For example
//
// EMP_SECECT_ARGS( (i,x,x,i,i), 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 )
//
// ... would return:
//
// 1,4,5,6,9,10,11,14,15

#define EMP_SELECT_i(A, ...) , A
#define EMP_SELECT_x(A, ...)

#define EMP_SELECT_do_arg(P, A) EMP_IMERGE_2(EMP_SELECT_, EMP_RUN_JOIN(EMP_GET_ARG_1, P))(A,~)

#define EMP_SELECT_ARGS(PATTERN, ...) EMP_POP_ARG( ~ EMP_ASSEMBLE_MACRO_1ARG(EMP_SELECT_ARGS_, PATTERN, __VA_ARGS__) )

#define EMP_SELECT_ARGS_1(P, A) EMP_SELECT_do_arg(P, A)
#define EMP_SELECT_ARGS_2(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_1( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_3(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_2( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_4(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_3( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_5(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_4( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_6(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_5( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_7(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_6( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_8(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_7( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_9(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_8( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_10(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_9( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_11(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_10( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_12(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_11( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_13(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_12( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_14(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_13( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_15(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_14( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_16(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_15( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_17(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_16( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_18(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_17( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_19(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_18( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_20(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_19( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_21(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_20( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_22(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_21( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_23(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_22( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_24(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_23( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_25(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_24( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_26(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_25( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_27(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_26( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_28(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_27( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_29(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_28( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_30(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_29( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_31(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_30( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_32(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_31( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_33(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_32( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_34(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_33( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_35(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_34( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_36(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_35( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_37(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_36( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_38(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_37( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_39(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_38( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_40(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_39( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_41(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_40( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_42(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_41( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_43(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_42( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_44(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_43( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_45(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_44( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_46(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_45( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_47(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_46( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_48(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_47( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_49(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_48( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_50(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_49( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_51(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_50( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_52(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_51( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_53(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_52( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_54(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_53( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_55(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_54( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_56(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_55( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_57(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_56( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_58(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_57( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_59(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_58( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_60(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_59( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_61(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_60( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_62(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_61( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_63(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_62( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_64(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_63( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_65(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_64( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_66(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_65( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_67(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_66( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_68(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_67( (EMP_ROTATE_ARGS P), __VA_ARGS__ )
#define EMP_SELECT_ARGS_69(P, A, ...) EMP_SELECT_do_arg(P, A)EMP_SELECT_ARGS_68( (EMP_ROTATE_ARGS P), __VA_ARGS__ )


#define EMP_GET_EVEN_ARGS(...) EMP_SELECT_ARGS((x,i), __VA_ARGS__)
#define EMP_GET_ODD_ARGS(...) EMP_SELECT_ARGS((i,x), __VA_ARGS__)


// Enable an arbitrary number of arguments (well, up to 10) to be merged AFTER being processed!
#define EMP_MERGE(...) EMP_ASSEMBLE_MACRO(EMP_MERGE_, __VA_ARGS__)
#define EMP_MERGE_1(A1) A1
#define EMP_MERGE_2(A1,A2) A1 ## A2
#define EMP_MERGE_3(A1,A2,A3) A1 ## A2 ## A3
#define EMP_MERGE_4(A1,A2,A3,A4) A1 ## A2 ## A3 ## A4
#define EMP_MERGE_5(A1,A2,A3,A4,A5) A1 ## A2 ## A3 ## A4 ## A5
#define EMP_MERGE_6(A1,A2,A3,A4,A5,A6) A1 ## A2 ## A3 ## A4 ## A5 ## A6
#define EMP_MERGE_7(A1,A2,A3,A4,A5,A6,A7) A1 ## A2 ## A3 ## A4 ## A5 ## A6 ## A7
#define EMP_MERGE_8(A1,A2,A3,A4,A5,A6,A7,A8) A1 ## A2 ## A3 ## A4 ## A5 ## A6 ## A7 ## A8
#define EMP_MERGE_9(A1,A2,A3,A4,A5,A6,A7,A8,A9) A1 ## A2 ## A3 ## A4 ## A5 ## A6 ## A7 ## A8 ## A9
#define EMP_MERGE_10(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) A1 ## A2 ## A3 ## A4 ## A5 ## A6 ## A7 ## A8 ## A9 ## A10

// Indirect merges to give a chance for arg evaluation...
#define EMP_IMERGE_2(A1, A2) EMP_MERGE_2(A1, A2)

// EMP_WRAP_EACH takes a wrapper macro and a variable set of arguments,
// then applied to wrapper macro to each argument in order.
#define EMP_WRAP_EACH(W, ...) EMP_ASSEMBLE_MACRO_2ARG(EMP_LAYOUT_, W, , __VA_ARGS__)

// EMP_LAYOUT takes a wrapper macro and padding information, wraps each argument in the macro
// and then spaces them out with the padding.
// W = Wrapper macro name, P = Padding between results
#define EMP_LAYOUT(W, P, ...) EMP_ASSEMBLE_MACRO_2ARG(EMP_LAYOUT_, W, P, __VA_ARGS__)
#define EMP_LAYOUT_0(W, P)
#define EMP_LAYOUT_1(W, P, A) W(A)
#define EMP_LAYOUT_2(W, P, A, ...) W(A) P EMP_LAYOUT_1(W, P, __VA_ARGS__)
#define EMP_LAYOUT_3(W, P, A, ...) W(A) P EMP_LAYOUT_2(W, P, __VA_ARGS__)
#define EMP_LAYOUT_4(W, P, A, ...) W(A) P EMP_LAYOUT_3(W, P, __VA_ARGS__)
#define EMP_LAYOUT_5(W, P, A, ...) W(A) P EMP_LAYOUT_4(W, P, __VA_ARGS__)
#define EMP_LAYOUT_6(W, P, A, ...) W(A) P EMP_LAYOUT_5(W, P, __VA_ARGS__)
#define EMP_LAYOUT_7(W, P, A, ...) W(A) P EMP_LAYOUT_6(W, P, __VA_ARGS__)
#define EMP_LAYOUT_8(W, P, A, ...) W(A) P EMP_LAYOUT_7(W, P, __VA_ARGS__)
#define EMP_LAYOUT_9(W, P, A, ...) W(A) P EMP_LAYOUT_8(W, P, __VA_ARGS__)
#define EMP_LAYOUT_10(W, P, A, ...) W(A) P EMP_LAYOUT_9(W, P, __VA_ARGS__)
#define EMP_LAYOUT_11(W, P, A, ...) W(A) P EMP_LAYOUT_10(W, P, __VA_ARGS__)
#define EMP_LAYOUT_12(W, P, A, ...) W(A) P EMP_LAYOUT_11(W, P, __VA_ARGS__)
#define EMP_LAYOUT_13(W, P, A, ...) W(A) P EMP_LAYOUT_12(W, P, __VA_ARGS__)
#define EMP_LAYOUT_14(W, P, A, ...) W(A) P EMP_LAYOUT_13(W, P, __VA_ARGS__)
#define EMP_LAYOUT_15(W, P, A, ...) W(A) P EMP_LAYOUT_14(W, P, __VA_ARGS__)
#define EMP_LAYOUT_16(W, P, A, ...) W(A) P EMP_LAYOUT_15(W, P, __VA_ARGS__)
#define EMP_LAYOUT_17(W, P, A, ...) W(A) P EMP_LAYOUT_16(W, P, __VA_ARGS__)
#define EMP_LAYOUT_18(W, P, A, ...) W(A) P EMP_LAYOUT_17(W, P, __VA_ARGS__)
#define EMP_LAYOUT_19(W, P, A, ...) W(A) P EMP_LAYOUT_18(W, P, __VA_ARGS__)
#define EMP_LAYOUT_20(W, P, A, ...) W(A) P EMP_LAYOUT_19(W, P, __VA_ARGS__)
#define EMP_LAYOUT_21(W, P, A, ...) W(A) P EMP_LAYOUT_20(W, P, __VA_ARGS__)
#define EMP_LAYOUT_22(W, P, A, ...) W(A) P EMP_LAYOUT_21(W, P, __VA_ARGS__)
#define EMP_LAYOUT_23(W, P, A, ...) W(A) P EMP_LAYOUT_22(W, P, __VA_ARGS__)
#define EMP_LAYOUT_24(W, P, A, ...) W(A) P EMP_LAYOUT_23(W, P, __VA_ARGS__)
#define EMP_LAYOUT_25(W, P, A, ...) W(A) P EMP_LAYOUT_24(W, P, __VA_ARGS__)
#define EMP_LAYOUT_26(W, P, A, ...) W(A) P EMP_LAYOUT_25(W, P, __VA_ARGS__)
#define EMP_LAYOUT_27(W, P, A, ...) W(A) P EMP_LAYOUT_26(W, P, __VA_ARGS__)
#define EMP_LAYOUT_28(W, P, A, ...) W(A) P EMP_LAYOUT_27(W, P, __VA_ARGS__)
#define EMP_LAYOUT_29(W, P, A, ...) W(A) P EMP_LAYOUT_28(W, P, __VA_ARGS__)
#define EMP_LAYOUT_30(W, P, A, ...) W(A) P EMP_LAYOUT_29(W, P, __VA_ARGS__)
#define EMP_LAYOUT_31(W, P, A, ...) W(A) P EMP_LAYOUT_30(W, P, __VA_ARGS__)
#define EMP_LAYOUT_32(W, P, A, ...) W(A) P EMP_LAYOUT_31(W, P, __VA_ARGS__)
#define EMP_LAYOUT_33(W, P, A, ...) W(A) P EMP_LAYOUT_32(W, P, __VA_ARGS__)
#define EMP_LAYOUT_34(W, P, A, ...) W(A) P EMP_LAYOUT_33(W, P, __VA_ARGS__)
#define EMP_LAYOUT_35(W, P, A, ...) W(A) P EMP_LAYOUT_34(W, P, __VA_ARGS__)
#define EMP_LAYOUT_36(W, P, A, ...) W(A) P EMP_LAYOUT_35(W, P, __VA_ARGS__)
#define EMP_LAYOUT_37(W, P, A, ...) W(A) P EMP_LAYOUT_36(W, P, __VA_ARGS__)
#define EMP_LAYOUT_38(W, P, A, ...) W(A) P EMP_LAYOUT_37(W, P, __VA_ARGS__)
#define EMP_LAYOUT_39(W, P, A, ...) W(A) P EMP_LAYOUT_38(W, P, __VA_ARGS__)
#define EMP_LAYOUT_40(W, P, A, ...) W(A) P EMP_LAYOUT_39(W, P, __VA_ARGS__)
#define EMP_LAYOUT_41(W, P, A, ...) W(A) P EMP_LAYOUT_40(W, P, __VA_ARGS__)
#define EMP_LAYOUT_42(W, P, A, ...) W(A) P EMP_LAYOUT_41(W, P, __VA_ARGS__)
#define EMP_LAYOUT_43(W, P, A, ...) W(A) P EMP_LAYOUT_42(W, P, __VA_ARGS__)
#define EMP_LAYOUT_44(W, P, A, ...) W(A) P EMP_LAYOUT_43(W, P, __VA_ARGS__)
#define EMP_LAYOUT_45(W, P, A, ...) W(A) P EMP_LAYOUT_44(W, P, __VA_ARGS__)
#define EMP_LAYOUT_46(W, P, A, ...) W(A) P EMP_LAYOUT_45(W, P, __VA_ARGS__)
#define EMP_LAYOUT_47(W, P, A, ...) W(A) P EMP_LAYOUT_46(W, P, __VA_ARGS__)
#define EMP_LAYOUT_48(W, P, A, ...) W(A) P EMP_LAYOUT_47(W, P, __VA_ARGS__)
#define EMP_LAYOUT_49(W, P, A, ...) W(A) P EMP_LAYOUT_48(W, P, __VA_ARGS__)
#define EMP_LAYOUT_50(W, P, A, ...) W(A) P EMP_LAYOUT_49(W, P, __VA_ARGS__)
#define EMP_LAYOUT_51(W, P, A, ...) W(A) P EMP_LAYOUT_50(W, P, __VA_ARGS__)
#define EMP_LAYOUT_52(W, P, A, ...) W(A) P EMP_LAYOUT_51(W, P, __VA_ARGS__)
#define EMP_LAYOUT_53(W, P, A, ...) W(A) P EMP_LAYOUT_52(W, P, __VA_ARGS__)
#define EMP_LAYOUT_54(W, P, A, ...) W(A) P EMP_LAYOUT_53(W, P, __VA_ARGS__)
#define EMP_LAYOUT_55(W, P, A, ...) W(A) P EMP_LAYOUT_54(W, P, __VA_ARGS__)
#define EMP_LAYOUT_56(W, P, A, ...) W(A) P EMP_LAYOUT_55(W, P, __VA_ARGS__)
#define EMP_LAYOUT_57(W, P, A, ...) W(A) P EMP_LAYOUT_56(W, P, __VA_ARGS__)
#define EMP_LAYOUT_58(W, P, A, ...) W(A) P EMP_LAYOUT_57(W, P, __VA_ARGS__)
#define EMP_LAYOUT_59(W, P, A, ...) W(A) P EMP_LAYOUT_58(W, P, __VA_ARGS__)
#define EMP_LAYOUT_60(W, P, A, ...) W(A) P EMP_LAYOUT_59(W, P, __VA_ARGS__)
#define EMP_LAYOUT_61(W, P, A, ...) W(A) P EMP_LAYOUT_60(W, P, __VA_ARGS__)
#define EMP_LAYOUT_62(W, P, A, ...) W(A) P EMP_LAYOUT_61(W, P, __VA_ARGS__)
#define EMP_LAYOUT_63(W, P, A, ...) W(A) P EMP_LAYOUT_62(W, P, __VA_ARGS__)


#define EMP_WRAP_ARGS(W, ...) EMP_ASSEMBLE_MACRO_1ARG(EMP_WRAP_ARGS_, W, __VA_ARGS__)
#define EMP_WRAP_ARGS_1(W, A) W(A)
#define EMP_WRAP_ARGS_2(W, A, ...) W(A), EMP_WRAP_ARGS_1(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_3(W, A, ...) W(A), EMP_WRAP_ARGS_2(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_4(W, A, ...) W(A), EMP_WRAP_ARGS_3(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_5(W, A, ...) W(A), EMP_WRAP_ARGS_4(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_6(W, A, ...) W(A), EMP_WRAP_ARGS_5(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_7(W, A, ...) W(A), EMP_WRAP_ARGS_6(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_8(W, A, ...) W(A), EMP_WRAP_ARGS_7(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_9(W, A, ...) W(A), EMP_WRAP_ARGS_8(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_10(W, A, ...) W(A), EMP_WRAP_ARGS_9(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_11(W, A, ...) W(A), EMP_WRAP_ARGS_10(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_12(W, A, ...) W(A), EMP_WRAP_ARGS_11(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_13(W, A, ...) W(A), EMP_WRAP_ARGS_12(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_14(W, A, ...) W(A), EMP_WRAP_ARGS_13(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_15(W, A, ...) W(A), EMP_WRAP_ARGS_14(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_16(W, A, ...) W(A), EMP_WRAP_ARGS_15(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_17(W, A, ...) W(A), EMP_WRAP_ARGS_16(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_18(W, A, ...) W(A), EMP_WRAP_ARGS_17(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_19(W, A, ...) W(A), EMP_WRAP_ARGS_18(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_20(W, A, ...) W(A), EMP_WRAP_ARGS_19(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_21(W, A, ...) W(A), EMP_WRAP_ARGS_20(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_22(W, A, ...) W(A), EMP_WRAP_ARGS_21(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_23(W, A, ...) W(A), EMP_WRAP_ARGS_22(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_24(W, A, ...) W(A), EMP_WRAP_ARGS_23(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_25(W, A, ...) W(A), EMP_WRAP_ARGS_24(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_26(W, A, ...) W(A), EMP_WRAP_ARGS_25(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_27(W, A, ...) W(A), EMP_WRAP_ARGS_26(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_28(W, A, ...) W(A), EMP_WRAP_ARGS_27(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_29(W, A, ...) W(A), EMP_WRAP_ARGS_28(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_30(W, A, ...) W(A), EMP_WRAP_ARGS_29(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_31(W, A, ...) W(A), EMP_WRAP_ARGS_30(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_32(W, A, ...) W(A), EMP_WRAP_ARGS_31(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_33(W, A, ...) W(A), EMP_WRAP_ARGS_32(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_34(W, A, ...) W(A), EMP_WRAP_ARGS_33(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_35(W, A, ...) W(A), EMP_WRAP_ARGS_34(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_36(W, A, ...) W(A), EMP_WRAP_ARGS_35(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_37(W, A, ...) W(A), EMP_WRAP_ARGS_36(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_38(W, A, ...) W(A), EMP_WRAP_ARGS_37(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_39(W, A, ...) W(A), EMP_WRAP_ARGS_38(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_40(W, A, ...) W(A), EMP_WRAP_ARGS_39(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_41(W, A, ...) W(A), EMP_WRAP_ARGS_40(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_42(W, A, ...) W(A), EMP_WRAP_ARGS_41(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_43(W, A, ...) W(A), EMP_WRAP_ARGS_42(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_44(W, A, ...) W(A), EMP_WRAP_ARGS_43(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_45(W, A, ...) W(A), EMP_WRAP_ARGS_44(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_46(W, A, ...) W(A), EMP_WRAP_ARGS_45(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_47(W, A, ...) W(A), EMP_WRAP_ARGS_46(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_48(W, A, ...) W(A), EMP_WRAP_ARGS_47(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_49(W, A, ...) W(A), EMP_WRAP_ARGS_48(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_50(W, A, ...) W(A), EMP_WRAP_ARGS_49(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_51(W, A, ...) W(A), EMP_WRAP_ARGS_50(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_52(W, A, ...) W(A), EMP_WRAP_ARGS_51(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_53(W, A, ...) W(A), EMP_WRAP_ARGS_52(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_54(W, A, ...) W(A), EMP_WRAP_ARGS_53(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_55(W, A, ...) W(A), EMP_WRAP_ARGS_54(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_56(W, A, ...) W(A), EMP_WRAP_ARGS_55(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_57(W, A, ...) W(A), EMP_WRAP_ARGS_56(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_58(W, A, ...) W(A), EMP_WRAP_ARGS_57(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_59(W, A, ...) W(A), EMP_WRAP_ARGS_58(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_60(W, A, ...) W(A), EMP_WRAP_ARGS_59(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_61(W, A, ...) W(A), EMP_WRAP_ARGS_60(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_62(W, A, ...) W(A), EMP_WRAP_ARGS_61(W, __VA_ARGS__)
#define EMP_WRAP_ARGS_63(W, A, ...) W(A), EMP_WRAP_ARGS_62(W, __VA_ARGS__)

// Wrap C different arguments.
#define EMP_WRAP_ARGSET(W, C, ...) EMP_ASSEMBLE_MACRO_2ARG(EMP_WRAP_ARGS_, W, C, __VA_ARGS__)
#define EMP_WRAP_ARGSET_0(W, C, ...)
#define EMP_WRAP_ARGSET_1(W, C, ...) W( EMP_CROP_ARGS_TO(C, __VA_ARGS__) ), \
                                                                                                     EMP_MERGE_2( EMP_WRAP_ARGSET_, EMP_SUB_1_ ## C ) (W, C, EMP_CROP_OFF(C, __VA_ARGS__) )
#define EMP_WRAP_ARGSET_2(W, C, ...) W(A), EMP_WRAP_ARGSET_1(W, C, __VA_ARGS__)
#define EMP_WRAP_ARGSET_3(W, C, ...) W(A), EMP_WRAP_ARGSET_2(W, C, __VA_ARGS__)


//Individually stringifies each variable passed to it and returns them
//with commas in between.
#define EMP_STRINGIFY_EACH(...) EMP_WRAP_ARGS(EMP_STRINGIFY, __VA_ARGS__)


#define EMP_WRAP_ARG_PAIRS(W, ...) EMP_ASSEMBLE_MACRO_1ARG(EMP_WRAP_ARG_PAIRS_, W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_2(W, A1, A2) W(A1, A2)
#define EMP_WRAP_ARG_PAIRS_4(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_2(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_6(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_4(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_8(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_6(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_10(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_8(W, __VA_ARGS__) 
#define EMP_WRAP_ARG_PAIRS_12(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_10(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_14(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_12(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_16(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_14(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_18(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_16(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_20(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_18(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_22(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_20(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_24(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_22(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_26(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_24(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_28(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_26(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_30(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_28(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_32(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_30(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_34(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_32(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_36(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_34(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_38(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_36(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_40(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_38(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_42(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_40(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_44(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_42(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_46(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_44(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_48(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_46(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_50(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_48(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_52(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_50(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_54(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_52(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_56(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_54(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_58(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_56(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_60(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_58(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_62(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_60(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_64(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_62(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_66(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_64(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_68(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_66(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_70(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_68(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_72(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_70(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_74(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_72(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_76(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_74(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_78(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_76(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_80(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_78(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_82(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_80(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_84(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_82(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_86(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_84(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_88(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_86(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_90(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_88(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_92(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_90(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_94(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_92(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_96(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_94(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_98(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_96(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_100(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_98(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_102(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_100(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_104(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_102(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_106(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_104(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_108(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_106(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_110(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_108(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_112(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_110(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_114(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_112(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_116(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_114(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_118(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_116(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_120(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_118(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_122(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_120(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_124(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_122(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_126(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_124(W, __VA_ARGS__)
#define EMP_WRAP_ARG_PAIRS_128(W, A1, A2, ...) W(A1, A2), EMP_WRAP_ARG_PAIRS_126(W, __VA_ARGS__)

#define EMP_WRAP_ARG_TRIPLES(W, ...) EMP_ASSEMBLE_MACRO_1ARG(EMP_WRAP_ARG_TRIPLES_, W, __VA_ARGS__)
#define EMP_WRAP_ARG_TRIPLES_3(W, A,B,C) W(A, B, C)
#define EMP_WRAP_ARG_TRIPLES_6(W, A,B,C, ...) W(A,B,C), EMP_WRAP_ARG_TRIPLES_3(W, __VA_ARGS__)
#define EMP_WRAP_ARG_TRIPLES_9(W, A,B,C, ...) W(A,B,C), EMP_WRAP_ARG_TRIPLES_6(W, __VA_ARGS__)
#define EMP_WRAP_ARG_TRIPLES_12(W, A,B,C, ...) W(A,B,C), EMP_WRAP_ARG_TRIPLES_9(W, __VA_ARGS__)

#define EMP_REVERSE_ARGS(...) EMP_ASSEMBLE_MACRO(EMP_REVERSE_ARGS_, __VA_ARGS__)
#define EMP_REVERSE_ARGS_1(A) A
#define EMP_REVERSE_ARGS_2(A, ...) EMP_REVERSE_ARGS_1(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_3(A, ...) EMP_REVERSE_ARGS_2(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_4(A, ...) EMP_REVERSE_ARGS_3(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_5(A, ...) EMP_REVERSE_ARGS_4(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_6(A, ...) EMP_REVERSE_ARGS_5(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_7(A, ...) EMP_REVERSE_ARGS_6(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_8(A, ...) EMP_REVERSE_ARGS_7(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_9(A, ...) EMP_REVERSE_ARGS_8(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_10(A, ...) EMP_REVERSE_ARGS_9(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_11(A, ...) EMP_REVERSE_ARGS_10(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_12(A, ...) EMP_REVERSE_ARGS_11(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_13(A, ...) EMP_REVERSE_ARGS_12(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_14(A, ...) EMP_REVERSE_ARGS_13(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_15(A, ...) EMP_REVERSE_ARGS_14(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_16(A, ...) EMP_REVERSE_ARGS_15(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_17(A, ...) EMP_REVERSE_ARGS_16(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_18(A, ...) EMP_REVERSE_ARGS_17(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_19(A, ...) EMP_REVERSE_ARGS_18(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_20(A, ...) EMP_REVERSE_ARGS_19(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_21(A, ...) EMP_REVERSE_ARGS_20(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_22(A, ...) EMP_REVERSE_ARGS_21(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_23(A, ...) EMP_REVERSE_ARGS_22(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_24(A, ...) EMP_REVERSE_ARGS_23(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_25(A, ...) EMP_REVERSE_ARGS_24(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_26(A, ...) EMP_REVERSE_ARGS_25(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_27(A, ...) EMP_REVERSE_ARGS_26(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_28(A, ...) EMP_REVERSE_ARGS_27(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_29(A, ...) EMP_REVERSE_ARGS_28(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_30(A, ...) EMP_REVERSE_ARGS_29(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_31(A, ...) EMP_REVERSE_ARGS_30(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_32(A, ...) EMP_REVERSE_ARGS_31(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_33(A, ...) EMP_REVERSE_ARGS_32(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_34(A, ...) EMP_REVERSE_ARGS_33(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_35(A, ...) EMP_REVERSE_ARGS_34(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_36(A, ...) EMP_REVERSE_ARGS_35(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_37(A, ...) EMP_REVERSE_ARGS_36(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_38(A, ...) EMP_REVERSE_ARGS_37(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_39(A, ...) EMP_REVERSE_ARGS_38(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_40(A, ...) EMP_REVERSE_ARGS_39(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_41(A, ...) EMP_REVERSE_ARGS_40(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_42(A, ...) EMP_REVERSE_ARGS_41(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_43(A, ...) EMP_REVERSE_ARGS_42(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_44(A, ...) EMP_REVERSE_ARGS_43(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_45(A, ...) EMP_REVERSE_ARGS_44(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_46(A, ...) EMP_REVERSE_ARGS_45(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_47(A, ...) EMP_REVERSE_ARGS_46(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_48(A, ...) EMP_REVERSE_ARGS_47(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_49(A, ...) EMP_REVERSE_ARGS_48(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_50(A, ...) EMP_REVERSE_ARGS_49(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_51(A, ...) EMP_REVERSE_ARGS_50(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_52(A, ...) EMP_REVERSE_ARGS_51(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_53(A, ...) EMP_REVERSE_ARGS_52(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_54(A, ...) EMP_REVERSE_ARGS_53(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_55(A, ...) EMP_REVERSE_ARGS_54(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_56(A, ...) EMP_REVERSE_ARGS_55(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_57(A, ...) EMP_REVERSE_ARGS_56(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_58(A, ...) EMP_REVERSE_ARGS_57(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_59(A, ...) EMP_REVERSE_ARGS_58(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_60(A, ...) EMP_REVERSE_ARGS_59(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_61(A, ...) EMP_REVERSE_ARGS_60(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_62(A, ...) EMP_REVERSE_ARGS_61(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_63(A, ...) EMP_REVERSE_ARGS_62(__VA_ARGS__), A
#define EMP_REVERSE_ARGS_64(A, ...) EMP_REVERSE_ARGS_63(__VA_ARGS__), A

#define EMP_TYPES_TO_ARGS(...) EMP_ASSEMBLE_MACRO(EMP_TYPES_TO_ARGS_, EMP_REVERSE_ARGS(__VA_ARGS__))
#define EMP_TYPES_TO_ARGS_1(A) A arg1
#define EMP_TYPES_TO_ARGS_2(A, ...) EMP_TYPES_TO_ARGS_1(__VA_ARGS__), A arg2
#define EMP_TYPES_TO_ARGS_3(A, ...) EMP_TYPES_TO_ARGS_2(__VA_ARGS__), A arg3
#define EMP_TYPES_TO_ARGS_4(A, ...) EMP_TYPES_TO_ARGS_3(__VA_ARGS__), A arg4
#define EMP_TYPES_TO_ARGS_5(A, ...) EMP_TYPES_TO_ARGS_4(__VA_ARGS__), A arg5
#define EMP_TYPES_TO_ARGS_6(A, ...) EMP_TYPES_TO_ARGS_5(__VA_ARGS__), A arg6
#define EMP_TYPES_TO_ARGS_7(A, ...) EMP_TYPES_TO_ARGS_6(__VA_ARGS__), A arg7
#define EMP_TYPES_TO_ARGS_8(A, ...) EMP_TYPES_TO_ARGS_7(__VA_ARGS__), A arg8
#define EMP_TYPES_TO_ARGS_9(A, ...) EMP_TYPES_TO_ARGS_8(__VA_ARGS__), A arg9
#define EMP_TYPES_TO_ARGS_10(A, ...) EMP_TYPES_TO_ARGS_9(__VA_ARGS__), A arg10
#define EMP_TYPES_TO_ARGS_11(A, ...) EMP_TYPES_TO_ARGS_10(__VA_ARGS__), A arg11
#define EMP_TYPES_TO_ARGS_12(A, ...) EMP_TYPES_TO_ARGS_11(__VA_ARGS__), A arg12
#define EMP_TYPES_TO_ARGS_13(A, ...) EMP_TYPES_TO_ARGS_12(__VA_ARGS__), A arg13
#define EMP_TYPES_TO_ARGS_14(A, ...) EMP_TYPES_TO_ARGS_13(__VA_ARGS__), A arg14
#define EMP_TYPES_TO_ARGS_15(A, ...) EMP_TYPES_TO_ARGS_14(__VA_ARGS__), A arg15
#define EMP_TYPES_TO_ARGS_16(A, ...) EMP_TYPES_TO_ARGS_15(__VA_ARGS__), A arg16
#define EMP_TYPES_TO_ARGS_17(A, ...) EMP_TYPES_TO_ARGS_16(__VA_ARGS__), A arg17
#define EMP_TYPES_TO_ARGS_18(A, ...) EMP_TYPES_TO_ARGS_17(__VA_ARGS__), A arg18
#define EMP_TYPES_TO_ARGS_19(A, ...) EMP_TYPES_TO_ARGS_18(__VA_ARGS__), A arg19
#define EMP_TYPES_TO_ARGS_20(A, ...) EMP_TYPES_TO_ARGS_19(__VA_ARGS__), A arg20
#define EMP_TYPES_TO_ARGS_21(A, ...) EMP_TYPES_TO_ARGS_20(__VA_ARGS__), A arg21
#define EMP_TYPES_TO_ARGS_22(A, ...) EMP_TYPES_TO_ARGS_21(__VA_ARGS__), A arg22
#define EMP_TYPES_TO_ARGS_23(A, ...) EMP_TYPES_TO_ARGS_22(__VA_ARGS__), A arg23
#define EMP_TYPES_TO_ARGS_24(A, ...) EMP_TYPES_TO_ARGS_23(__VA_ARGS__), A arg24
#define EMP_TYPES_TO_ARGS_25(A, ...) EMP_TYPES_TO_ARGS_24(__VA_ARGS__), A arg25
#define EMP_TYPES_TO_ARGS_26(A, ...) EMP_TYPES_TO_ARGS_25(__VA_ARGS__), A arg26
#define EMP_TYPES_TO_ARGS_27(A, ...) EMP_TYPES_TO_ARGS_26(__VA_ARGS__), A arg27
#define EMP_TYPES_TO_ARGS_28(A, ...) EMP_TYPES_TO_ARGS_27(__VA_ARGS__), A arg28
#define EMP_TYPES_TO_ARGS_29(A, ...) EMP_TYPES_TO_ARGS_28(__VA_ARGS__), A arg29
#define EMP_TYPES_TO_ARGS_30(A, ...) EMP_TYPES_TO_ARGS_29(__VA_ARGS__), A arg30
#define EMP_TYPES_TO_ARGS_31(A, ...) EMP_TYPES_TO_ARGS_30(__VA_ARGS__), A arg31
#define EMP_TYPES_TO_ARGS_32(A, ...) EMP_TYPES_TO_ARGS_31(__VA_ARGS__), A arg32
#define EMP_TYPES_TO_ARGS_33(A, ...) EMP_TYPES_TO_ARGS_32(__VA_ARGS__), A arg33
#define EMP_TYPES_TO_ARGS_34(A, ...) EMP_TYPES_TO_ARGS_33(__VA_ARGS__), A arg34
#define EMP_TYPES_TO_ARGS_35(A, ...) EMP_TYPES_TO_ARGS_34(__VA_ARGS__), A arg35
#define EMP_TYPES_TO_ARGS_36(A, ...) EMP_TYPES_TO_ARGS_35(__VA_ARGS__), A arg36
#define EMP_TYPES_TO_ARGS_37(A, ...) EMP_TYPES_TO_ARGS_36(__VA_ARGS__), A arg37
#define EMP_TYPES_TO_ARGS_38(A, ...) EMP_TYPES_TO_ARGS_37(__VA_ARGS__), A arg38
#define EMP_TYPES_TO_ARGS_39(A, ...) EMP_TYPES_TO_ARGS_38(__VA_ARGS__), A arg39
#define EMP_TYPES_TO_ARGS_40(A, ...) EMP_TYPES_TO_ARGS_39(__VA_ARGS__), A arg40
#define EMP_TYPES_TO_ARGS_41(A, ...) EMP_TYPES_TO_ARGS_40(__VA_ARGS__), A arg41
#define EMP_TYPES_TO_ARGS_42(A, ...) EMP_TYPES_TO_ARGS_41(__VA_ARGS__), A arg42
#define EMP_TYPES_TO_ARGS_43(A, ...) EMP_TYPES_TO_ARGS_42(__VA_ARGS__), A arg43
#define EMP_TYPES_TO_ARGS_44(A, ...) EMP_TYPES_TO_ARGS_43(__VA_ARGS__), A arg44
#define EMP_TYPES_TO_ARGS_45(A, ...) EMP_TYPES_TO_ARGS_44(__VA_ARGS__), A arg45
#define EMP_TYPES_TO_ARGS_46(A, ...) EMP_TYPES_TO_ARGS_45(__VA_ARGS__), A arg46
#define EMP_TYPES_TO_ARGS_47(A, ...) EMP_TYPES_TO_ARGS_46(__VA_ARGS__), A arg47
#define EMP_TYPES_TO_ARGS_48(A, ...) EMP_TYPES_TO_ARGS_47(__VA_ARGS__), A arg48
#define EMP_TYPES_TO_ARGS_49(A, ...) EMP_TYPES_TO_ARGS_48(__VA_ARGS__), A arg49
#define EMP_TYPES_TO_ARGS_50(A, ...) EMP_TYPES_TO_ARGS_49(__VA_ARGS__), A arg50
#define EMP_TYPES_TO_ARGS_51(A, ...) EMP_TYPES_TO_ARGS_50(__VA_ARGS__), A arg51
#define EMP_TYPES_TO_ARGS_52(A, ...) EMP_TYPES_TO_ARGS_51(__VA_ARGS__), A arg52
#define EMP_TYPES_TO_ARGS_53(A, ...) EMP_TYPES_TO_ARGS_52(__VA_ARGS__), A arg53
#define EMP_TYPES_TO_ARGS_54(A, ...) EMP_TYPES_TO_ARGS_53(__VA_ARGS__), A arg54
#define EMP_TYPES_TO_ARGS_55(A, ...) EMP_TYPES_TO_ARGS_54(__VA_ARGS__), A arg55
#define EMP_TYPES_TO_ARGS_56(A, ...) EMP_TYPES_TO_ARGS_55(__VA_ARGS__), A arg56
#define EMP_TYPES_TO_ARGS_57(A, ...) EMP_TYPES_TO_ARGS_56(__VA_ARGS__), A arg57
#define EMP_TYPES_TO_ARGS_58(A, ...) EMP_TYPES_TO_ARGS_57(__VA_ARGS__), A arg58
#define EMP_TYPES_TO_ARGS_59(A, ...) EMP_TYPES_TO_ARGS_58(__VA_ARGS__), A arg59
#define EMP_TYPES_TO_ARGS_60(A, ...) EMP_TYPES_TO_ARGS_59(__VA_ARGS__), A arg60
#define EMP_TYPES_TO_ARGS_61(A, ...) EMP_TYPES_TO_ARGS_60(__VA_ARGS__), A arg61
#define EMP_TYPES_TO_ARGS_62(A, ...) EMP_TYPES_TO_ARGS_61(__VA_ARGS__), A arg62
#define EMP_TYPES_TO_ARGS_63(A, ...) EMP_TYPES_TO_ARGS_62(__VA_ARGS__), A arg63


// Setup a generic method of calling a specific version of a macro based on argument count.
// If some of the args need to be passed to each version, specify number in macro call.
#define EMP_ASSEMBLE_MACRO(BASE, ...)                                   \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_1ARG(BASE, A, ...)                           \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_2ARG(BASE, A, B, ...)                         \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_3ARG(BASE, A, B, C, ...)                      \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_4ARG(BASE, A, B, C, D, ...)                   \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, D, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_5ARG(BASE, A, B, C, D, E, ...)                \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, D, E, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_6ARG(BASE, A, B, C, D, E, F, ...)             \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, D, E, F, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_7ARG(BASE, A, B, C, D, E, F, G, ...)          \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, D, E, F, G, __VA_ARGS__)

#define EMP_ASSEMBLE_MACRO_8ARG(BASE, A, B, C, D, E, F, G, H, ...)       \
  EMP_ASSEMBLE_IMPL(BASE, EMP_COUNT_ARGS(__VA_ARGS__), A, B, C, D, E, F, G, H, __VA_ARGS__)

#define EMP_ASSEMBLE_IMPL(BASE, ARG_COUNT, ...) EMP_ASSEMBLE_MERGE(BASE, ARG_COUNT) (__VA_ARGS__)
#define EMP_ASSEMBLE_MERGE(A, B) A ## B

#endif
