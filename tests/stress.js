// STRESS TESTS:
//      Go above the limits of the server

import http from 'k6/http';
import {sleep} from 'k6';

export let options = {
    insecureSkipTLSVerify: true,
    noConnectionReuse: false,
    stages: [
        { duration: '10s', target: 20}, // ramp-up of traffic from 0 to 20 users in 10 seconds
        { duration: '15s', target: 1000}, // ramp-up to 1000 users in 15 seconds
        { duration: '10s', target: 1000}, // stays at 1000 users for 10 seconds
        { duration: '15s', target: 20}, // ramps-down to 20 users in 15 seconds
        { duration: '10s', target: 0} // ramp-down to 0 users
    ],
    thresholds: {
        http_req_duration: ['p(99)<150'] // 99% of requests must complete below 150ms
    }
}

const API_BASE_URL = 'http://localhost:8000';

export default () => {

    http.batch([
        ['GET', `${API_BASE_URL}/`],
        ['GET', `${API_BASE_URL}/about`],
        ['GET', `${API_BASE_URL}/send-files`],
        ['GET', `${API_BASE_URL}/storage`],
        ['GET', `${API_BASE_URL}/form`]
    ]);
    sleep(1);
}
