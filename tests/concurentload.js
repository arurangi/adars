// CONCURENT STRESS TESTS:
//      Tests simultenuous connections on different ports

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

const API_BASE_URL = 'http://localhost:';

function getRandomPage(min, max) {
    pages = ['/', '/about', '/send-files', 'form']
    idx = Math.floor(Math.random() * (max - min + 1)) + min;
    return pages[idx];
}

export default () => {

    http.batch([
        ['GET', `${API_BASE_URL}:8000/`],
        ['GET', `${API_BASE_URL}:8001/`],
        ['GET', `${API_BASE_URL}:8002/`],
    ]);

    // http.batch([
    //     ['GET', `${API_BASE_URL}:8000/`],
    //     ['GET', `${API_BASE_URL}:8001/${getRandomPage(0, 4)}`],
    //     ['GET', `${API_BASE_URL}:8002/${getRandomPage(0, 4)}`],
    // ]);

    // http.batch([
    //     ['GET', `${API_BASE_URL}:8000/`],
    //     ['GET', `${API_BASE_URL}:8001/about`],
    //     ['GET', `${API_BASE_URL}:8002/send-files`],
    // ]);

    sleep(1);
}
