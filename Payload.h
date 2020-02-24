/*
 * Structure.h
 *
 *  Created on: Feb 25, 2020
 *      Author: user
 */

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <napi.h>

struct Payload{

	int time;

	Napi::Object toMsgObject(Napi::Env &env) const {
		auto msg = Napi::Object::New(env);
		auto payload = Napi::Object::New(env);
		payload.Set("time", time);
		msg.Set("payload", payload);

		return msg;
	}
};



#endif /* PAYLOAD_H_ */
