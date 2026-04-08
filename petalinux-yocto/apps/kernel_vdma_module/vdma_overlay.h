/**
 * Copyright (C) 2025 Oliwier Jaworski 
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#define FB_ADDR(vinfo) vinfo->fb_info.addr

#define VSIZE 1080
#define HSIZE 1920 
#define RGBA 4
#define STRIDE (HSIZE*RGBA)
#define BUFFER_SIZE (VSIZE* HSIZE* 4)	 	/* must match driver exactly */

