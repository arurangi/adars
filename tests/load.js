// LOAD TESTS:
//      Checks how the server handles expected traffic

import http from 'k6/http';
import {sleep} from 'k6';

export let options = {
    insecureSkipTLSVerify: true,
    noConnectionReuse: false,
    stages: [
        { duration: '20s', target: 100}, // ramp-up of traffic from 0 to 20 users in 5 seconds
        { duration: '15s', target: 100}, // stay at 20 users for 15 seconds
        { duration: '20s', target: 0} // ramp-down to 0 users
    ],
    thresholds: {
        http_req_duration: ['p(99)<150'] // 99% of requests must complete below 150ms
    }
}

const API_BASE_URL = 'http://localhost:8000';

export default () => {

    http.get(`${API_BASE_URL}/`);

    // http.batch([
    //     ['GET', `${API_BASE_URL}/`],
    //     ['GET', `${API_BASE_URL}/about`],
    //     ['GET', `${API_BASE_URL}/send-files`],
    //     ['GET', `${API_BASE_URL}/storage`],
    //     ['GET', `${API_BASE_URL}/form`]
    // ]);

    sleep(1);
}
