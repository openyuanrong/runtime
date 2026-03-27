/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.yuanrong.services.session;

import org.yuanrong.errorcode.ErrorCode;
import org.yuanrong.errorcode.ModuleCode;
import org.yuanrong.exception.LibRuntimeException;
import org.yuanrong.exception.YRException;
import org.yuanrong.jni.LibRuntime;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Runtime-managed implementation of {@link SessionObj}.
 *
 * <p>Constructed by the JNI layer after loading from libruntime.
 * {@link #setHistories(List)} immediately syncs the change back to libruntime
 * via {@code UpdateCurrentSession} so the runtime holds the latest data
 * before the final persist triggered by C++ InvokeAdaptor.</p>
 *
 * @since 2026/03/25
 */
public class ManagedSessionObj implements SessionObj {
    private static final Gson GSON = new GsonBuilder().create();

    private final String id;
    private List<String> histories;

    /**
     * Constructed by JNI / {@link SessionServiceImpl}.
     *
     * @param id        session ID
     * @param histories initial history list (may be empty, must not be null)
     */
    public ManagedSessionObj(String id, List<String> histories) {
        this.id = id == null ? "" : id;
        this.histories = histories == null ? new ArrayList<>() : new ArrayList<>(histories);
    }

    @Override
    public String getID() {
        return id;
    }

    /**
     * Returns an unmodifiable view of the history list.
     *
     * <p>Direct modification of the returned list is ignored by the runtime.
     * Use {@link #setHistories(List)} to persist changes.</p>
     */
    @Override
    public List<String> getHistories() {
        return Collections.unmodifiableList(histories);
    }

    /**
     * Replaces the history list and immediately notifies libruntime.
     *
     * @param histories new history list (null treated as empty list)
     * @throws YRException if the native update call fails
     */
    @Override
    public void setHistories(List<String> histories) throws YRException {
        this.histories = histories == null ? new ArrayList<>() : new ArrayList<>(histories);
        String sessionJson = serialize();
        try {
            LibRuntime.updateCurrentSession(id, sessionJson);
        } catch (LibRuntimeException e) {
            throw new YRException(e.getErrorCode(), e.getModuleCode(), e.getMessage());
        }
    }

    /**
     * Serialize to the canonical JSON format stored in DataSystem.
     *
     * <pre>{"sessionID":"s-123","histories":["user: hello","assistant: hi"]}</pre>
     */
    private String serialize() {
        SessionJsonDto dto = new SessionJsonDto(id, histories);
        return GSON.toJson(dto);
    }

    /**
     * Deserialize from the canonical JSON format returned by libruntime.
     *
     * <pre>{"sessionID":"s-123","histories":["user: hello","assistant: hi"]}</pre>
     *
     * @param json session JSON string (may be null or empty)
     * @return ManagedSessionObj, or null if json is null/empty
     */
    public static ManagedSessionObj fromJson(String json) {
        if (json == null || json.isEmpty()) {
            return null;
        }
        SessionJsonDto dto = GSON.fromJson(json, SessionJsonDto.class);
        if (dto == null) {
            return null;
        }
        List<String> h = dto.histories != null ? dto.histories : new ArrayList<>();
        return new ManagedSessionObj(dto.sessionID, h);
    }

    /** Private DTO used only for JSON serialization/deserialization. */
    private static class SessionJsonDto {
        private String sessionID;
        private List<String> histories;

        SessionJsonDto(String sessionID, List<String> histories) {
            this.sessionID = sessionID;
            this.histories = histories;
        }
    }
}
